#include <nac.h>
#include <exception>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <limits>
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
    nac_tensor_get_info(ti, &info);
    tsize = info.w*info.h*info.c*info.n;
    if(tsize != (bi.length/sizeof(float))){
        std::cerr<<"file"<<bi.file_name<<", "<<bi.length<<", not same as request tensor size:"<<tsize<<std::endl;
        std::terminate();
    }
    rtn = nac_tensor_copy_data(bi.ptr, 0, ti, 0, bi.length/sizeof(float), TENSOR_COPY_H2D);    
    if(rtn != NAC_SUCCESS)
        std::terminate();

    // output tensor
    nac_tensor_get_info(to, &info);
    tsize = info.w*info.h*info.c*info.n;
    if(tsize != (bo.length/sizeof(float))){
        std::cerr<<"file"<<bo.file_name<<", "<<bo.length<<", not same as request tensor size:"<<tsize<<std::endl;
        std::terminate();
    }
    rtn = nac_tensor_copy_data(bo.ptr, 0, to, 0, bo.length/sizeof(float), TENSOR_COPY_H2D);    
    if(rtn != NAC_SUCCESS)
        std::terminate();


    // darknet weight contains 
    // <n>-bias + <n>-scales + <n>-rolling_mean + <n>-rolling_variance +
    // <size*size * c * n>-weight

    float * ptr_w = (float *)bw.ptr;
    float * new_bias = new float [64];
    {
        // merge bn into bias
        float * bias = ptr_w;
        float * scale = &ptr_w[64];
        float * mean = &ptr_w[64*2];
        float * variance = &ptr_w[64*3];  //sqrt(l->rolling_variance[i])+.000001f;  
        for(int i=0;i<64;i++){
            float var = std::sqrt(variance[i]) + .000001f;
            new_bias[i] = bias[i] - mean[i]*scale[i]/var;
        }
    }
    rtn = nac_tensor_copy_data(new_bias, 0, tb, 0, 64, TENSOR_COPY_H2D);    
    if(rtn != NAC_SUCCESS)
        std::terminate();

    float * weight = &ptr_w[4*64];
    float * new_weight = new float [3*3*32*64];
    {
        float * scale = &ptr_w[64];
        float * variance = &ptr_w[64*3];
        for(int i=0;i<64;i++){
            float var = std::sqrt(variance[i]) + .000001f;
            for(int j=0;j<3*3* 32;j++){
                new_weight[i*3*3* 32 + j] = weight[i*3*3* 32 + j] * scale[i]  / var;
            }
        }
    }
    rtn = nac_tensor_copy_data(new_weight, 0, tw, 0, 3*3* 32 * 64, TENSOR_COPY_H2D);    
    if(rtn != NAC_SUCCESS)
        std::terminate();

    delete [] new_bias;
    delete [] new_weight;

    return 0;
}

int test_conv(const std::string & op_name){
    nac_node test_node = nac_node_create(g_context, g_op_entry, op_name.c_str());
    nac_graph test_graph = nac_graph_create(g_context);
    nac_hparam conv_hparam = nac_hparam_create(op_name.c_str());
    // layer-02 in yolov2
    nac_hparam_set(conv_hparam, "kernel", "3");
    nac_hparam_set(conv_hparam, "padding", "1");
    nac_hparam_set(conv_hparam, "stride", "1");
    nac_hparam_set(conv_hparam, "filters", "64");
    nac_hparam_set(conv_hparam, "activation", "leaky");

    nac_tensor ti = nac_tensor_create(g_op_entry,208,208,32,1);
    nac_tensor to = nac_tensor_create(g_op_entry,208,208,64,1);
    nac_tensor _to = nac_tensor_create(g_op_entry,208,208,64,1);
    nac_tensor tw = nac_tensor_create(g_op_entry,3,3,32,64);  // size*size, c, n
    nac_tensor tb = nac_tensor_create(g_op_entry,1,1,1,64);
    if(load_conv_tensors_yolov2_l2(ti, to, tw, tb) != 0)
        std::terminate();

    nac_tensor w[2] = {tw, tb};
    if(NAC_SUCCESS != nac_node_feed_weights(test_node, w, 2))
        std::terminate();

    if(NAC_SUCCESS != nac_node_set_hparam(test_node, conv_hparam))
        std::terminate();

    if(NAC_SUCCESS != nac_graph_attach_node(test_graph, &test_node, 1))
        std::terminate();

    if(NAC_SUCCESS != nac_graph_feed_inputs(test_graph,&ti, 1))
        std::terminate();

    if(NAC_SUCCESS != nac_graph_prepare(test_graph))
        std::terminate();

    // inference
    if(NAC_SUCCESS != nac_graph_start_inference(test_graph, 1, 1))
        std::terminate();

    if(NAC_SUCCESS != nac_graph_get_result(test_graph, &_to))
        std::terminate();

    // compare with golden
    float * out, * _out;
    out = (float*)nac_tensor_get_data_raw(to);  // golden
    _out = (float*)nac_tensor_get_data_raw(_to);

    bool fail = false;
    float delta_max = -std::numeric_limits<float>::infinity();
    for(int i=0;i<208*208*64;i++){
        float delta = out[i] - _out[i];
        if(delta > 0.0001f){
            fail = true;
            std::cerr<<"====== inference error idx:"<<i<<", delta:"<<delta;
            std::cerr<<", gloden:"<<out[i]<<", inference:"<<_out[i]<<std::endl;
        }
        if(delta > delta_max)
            delta_max = delta;
    }
    std::cout<<"  ... max delta:"<<delta_max<<std::endl;

    nac_hparam_release(conv_hparam);
    nac_graph_release(test_graph);
    //nac_tensor_release(ti);
    nac_tensor_release(to);     // only handle golden tensor release
    //nac_tensor_release(tw);
    //nac_tensor_release(tb);
    //nac_tensor_release(_to);

    return fail?-1:0;
}

static int load_maxpool_tensors_yolov2_l7(nac_tensor & ti, nac_tensor & to){
    blob_struct bi("layer-07-i-maxpool-104-104-128-1-fp32.bin");
    blob_struct bo("layer-07-o-maxpool-52-52-128-1-fp32.bin");

    nac_status rtn = nac_tensor_copy_data(bi.ptr, 0, ti, 0, bi.length/sizeof(float), TENSOR_COPY_H2D);    
    if(rtn != NAC_SUCCESS)
        std::terminate();

    rtn = nac_tensor_copy_data(bo.ptr, 0, to, 0, bo.length/sizeof(float), TENSOR_COPY_H2D);    
    if(rtn != NAC_SUCCESS)
        std::terminate();

    return 0;
}
int test_maxpool(const std::string & op_name){
    nac_tensor ti = nac_tensor_create(g_op_entry,104,104,128,1);
    nac_tensor to = nac_tensor_create(g_op_entry, 52, 52,128,1);
    nac_tensor _to = nac_tensor_create(g_op_entry, 52, 52,128,1);

    load_maxpool_tensors_yolov2_l7(ti, to);

    nac_hparam mp_hparam = nac_hparam_create(op_name.c_str());
    // layer-07 in yolov2
    nac_hparam_set(mp_hparam, "kernel", "2");
    nac_hparam_set(mp_hparam, "padding", "0");
    nac_hparam_set(mp_hparam, "stride", "2");

    nac_node test_node = nac_node_create(g_context, g_op_entry, op_name.c_str());
    nac_graph test_graph = nac_graph_create(g_context);

    if(NAC_SUCCESS != nac_node_set_hparam(test_node, mp_hparam))
        std::terminate();

    if(NAC_SUCCESS != nac_graph_attach_node(test_graph, &test_node, 1))
        std::terminate();

    if(NAC_SUCCESS != nac_graph_feed_inputs(test_graph,&ti, 1))
        std::terminate();

    if(NAC_SUCCESS != nac_graph_prepare(test_graph))
        std::terminate();

    // inference
    if(NAC_SUCCESS != nac_graph_start_inference(test_graph, 1, 1))
        std::terminate();

    if(NAC_SUCCESS != nac_graph_get_result(test_graph, &_to))
        std::terminate();

    // compare with golden
    float * out, * _out;
    out = (float*)nac_tensor_get_data_raw(to);  // golden
    _out = (float*)nac_tensor_get_data_raw(_to);

    bool fail = false;
    float delta_max = -std::numeric_limits<float>::infinity();
    for(int i=0;i<52*52*128;i++){
        float delta = out[i] - _out[i];
        if(delta > 0.0001f){
            fail = true;
            std::cerr<<"====== inference error idx:"<<i<<", delta:"<<delta;
            std::cerr<<", gloden:"<<out[i]<<", inference:"<<_out[i]<<std::endl;
        }
        if(delta > delta_max)
            delta_max = delta;
    }
    std::cout<<"  ... max delta:"<<delta_max<<std::endl;


    nac_hparam_release(mp_hparam);
    nac_graph_release(test_graph);
    nac_tensor_release(to);     // only handle golden tensor release

    return fail?-1:0;
}

/*------------------------------------------------------*/
static struct {
    std::string op_name;
    int (*test)(const std::string &);
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
    ret = nac_device_get(&devs, &num_devs);
    if(ret != NAC_SUCCESS)
        std::terminate();

    for(i=0;i<num_devs;i++){
        nac_device_info di;
        ret = nac_device_get_info(devs[i], &di );
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
    ret = nac_device_get_info(g_device, &dev_info);
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
    g_op_entry = nac_op_entry_get(g_device, dev_info.op_entry_names[i]);
    if(!g_op_entry){
        std::cout<< "fail to get op entry:"<<dev_info.op_entry_names[i]<<std::endl;
        std::terminate();
    }

    // context
    g_context = nac_context_create(&g_device, 1);
    if(!g_context){
        std::cout<< "fail to create context"<<std::endl;
        std::terminate();
    }
}


void test(){
    int test_total = sizeof(g_test_plan) / sizeof(g_test_plan[0]);
    for(int i=0;i<test_total;i++){
        std::cout<<"Testing:"<<g_test_plan[i].op_name<<std::endl;
        int result = g_test_plan[i].test(g_test_plan[i].op_name);
        if(result == 0)
            std::cout<<"    ...success"<<std::endl;
        else
            std::cout<<"    ...fail"<<std::endl;
    }
}

void finish(){
    nac_context_release(g_context);
}


int main(int argc, char ** argv){
    prepare();
    test();
    finish();

    return 0;
}
