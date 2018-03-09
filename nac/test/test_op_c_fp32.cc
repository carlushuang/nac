#include <nac.h>
#include <exception>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
nac_device      g_device;
nac_context     g_context;
nac_op_entry    g_op_entry;



static std::string wrap_weight_file(const char * pure_fname){
    char * var = std::getenv("WDIR");
    if(!var)
        return pure_fname;
    std::string full_name(var);
    full_name += "/";
    full_name += pure_fname;
    return full_name;
}

static int file_len(std::ifstream & f){
    f.seekg (0, std::ios::end);
    int length = f.tellg();
    f.seekg (0, std::ios::beg);
    return length;
}

struct blob_struct{
    const char *    file_name;
    std::string     full_name;
    int             length;
    char *          ptr;

    blob_struct(const char * fname) : file_name(fname){
        std::ifstream   fs;
        full_name = wrap_weight_file(fname);
        fs.open(full_name.c_str(), std::ios::in|std::ios::binary);
        if(!fs){
            std::cerr<<"open file:"<<full_name<<" fail, "<<std::endl;
            std::terminate();
        }
        length = file_len(fs);
        ptr  = new char[length];
        fs.read(ptr, length);
        if(!fs){
            std::cerr<<"error while read file:"<<full_name<<std::endl;
            std::terminate();
        }
        fs.close();
    }
    ~blob_struct(){
        //fs.close();
        if(ptr)
            delete [] ptr;
    }
};

static int load_conv_tensors_yolov2_l2(nac_tensor & ti,nac_tensor & to,nac_tensor & tw,nac_tensor & tb){
    blob_struct bi("layer-02-i-convolutional-208-208-32-1-fp32.bin");
    blob_struct bo("layer-02-o-convolutional-208-208-64-1-fp32.bin");
    blob_struct bw("weight-02-convolutional-3-64-fp32.bin");

    nac_status rtn;
    nac_tensor_info info;
    int             tsize;

    // input tensor
    nac_get_tensor_info(ti, &info);
    tsize = info.w*info.h*info.c*info.n;
    if(tsize != (bi.length/sizeof(float))){
        std::cerr<<"file"<<bi.file_name<<", "<<bi.length<<", not same as request tensor size:"<<tsize<<std::endl;
        std::terminate();
    }
    rtn = nac_copy_tensor_data(bi.ptr, 0, ti, 0, bi.length/sizeof(float), TENSOR_COPY_H2D);    
    if(rtn != NAC_SUCCESS)
        std::terminate();
    
    // output tensor
    nac_get_tensor_info(to, &info);
    tsize = info.w*info.h*info.c*info.n;
    if(tsize != (bo.length/sizeof(float))){
        std::cerr<<"file"<<bo.file_name<<", "<<bo.length<<", not same as request tensor size:"<<tsize<<std::endl;
        std::terminate();
    }
    rtn = nac_copy_tensor_data(bo.ptr, 0, to, 0, bo.length/sizeof(float), TENSOR_COPY_H2D);    
    if(rtn != NAC_SUCCESS)
        std::terminate();


    // darknet weight contains 
    // <n>-bias + <n>-scales + <n>-rolling_mean + <n>-rolling_variance +
    // <size*size * c * n>-weight

    float * ptr_w = (float *)bw.ptr;
    float * new_bias = new float [64];

    return 0;
}

void test_conv(const std::string & op_name){
    nac_node test_node = nac_create_node(g_context, g_op_entry, op_name.c_str());
    nac_graph test_graph = nac_create_graph(g_context);
    nac_hparam conv_hparam = nac_create_hparam(op_name.c_str());
    // layer-02 in yolov2
    nac_set_hparam(conv_hparam, "kernel", "3");
    nac_set_hparam(conv_hparam, "padding", "1");
    nac_set_hparam(conv_hparam, "stride", "1");
    nac_set_hparam(conv_hparam, "filters", "64");
    nac_set_hparam(conv_hparam, "activation", "leaky");
    nac_graph_attach_node(test_graph, &test_node, 1);

    nac_tensor ti = nac_create_tensor(g_op_entry,208,208,32,1);
    nac_tensor to = nac_create_tensor(g_op_entry,208,208,64,1);
    nac_tensor tw = nac_create_tensor(g_op_entry,3*3,32,64,1);  // size*size, c, n
    nac_tensor tb = nac_create_tensor(g_op_entry,64,1,1,1);
    if(load_conv_tensors_yolov2_l2(ti, to, tw, tb) != 0)
        std::terminate();

}
void test_maxpool(const std::string & op_name){

}

/*------------------------------------------------------*/
static struct {
    std::string op_name;
    void (*test)(const std::string &);
} g_test_plan[] = {
    {"conv",        test_conv},
    {"maxpool",     test_maxpool}
};

void prepare(){
    nac_status ret;
    nac_device * devs;
    int num_devs;
    int i;

    // device
    ret = nac_get_devices(&devs, &num_devs);
    if(ret != NAC_SUCCESS)
        std::terminate();

    for(i=0;i<num_devs;i++){
        nac_device_info di;
        ret = nac_get_device_info(devs[i], &di );
        if(ret != NAC_SUCCESS)
            std::terminate();
        std::string _dn(di.dev_name);
        if(_dn == "c")
            break;
    }
    if(i == num_devs){
        std::cout<< "can not find device \"c\" "<<std::endl;
        std::terminate();
    }
    g_device = devs[i];

    struct nac_device_info dev_info;
    ret = nac_get_device_info(g_device, &dev_info);
    if(ret != NAC_SUCCESS)
        std::terminate();
    
    std::cout<<"device: "<<dev_info.dev_name<<std::endl;
    std::cout<<"    op entry: ";
    for(i=0;i<dev_info.num_op_entries;i++)
        std::cout<<dev_info.op_entry_names[i]<<" ";
    std::cout<<std::endl;

    // op_entry
    for(i=0;i<dev_info.num_op_entries;i++){
        std::string s("fp32");
        if(dev_info.op_entry_names[i] == s)
            break;
    }
    if(i == dev_info.num_op_entries){
        std::cout<< "can not find op entry \"fp32\" "<<std::endl;
        std::terminate();
    }
    g_op_entry = nac_get_op_entry(g_device, dev_info.op_entry_names[i]);
    if(!g_op_entry){
        std::cout<< "fail to get op entry:"<<dev_info.op_entry_names[i]<<std::endl;
        std::terminate();
    }

    // context
    g_context = nac_create_context(&g_device, 1);
    if(!g_context){
        std::cout<< "fail to create context"<<std::endl;
        std::terminate();
    }
}


void test(){
    int test_total = sizeof(g_test_plan) / sizeof(g_test_plan[0]);
    for(int i=0;i<test_total;i++){
        std::cout<<"Testing:"<<g_test_plan[i].op_name<<std::endl;
        g_test_plan[i].test(g_test_plan[i].op_name);
    }
}

void finish(){
    nac_release_context(g_context);
}


int main(int argc, char ** argv){
    prepare();
    test();
    finish();

    return 0;
}
