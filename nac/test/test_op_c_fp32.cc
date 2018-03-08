#include <nac.h>
#include <exception>
#include <string>
#include <iostream>

using namespace nac;

nac_device      g_device;
nac_context     g_context;
nac_op_entry    g_op_entry;

void test_conv(const std::string & op_name){
    nac_node test_node = nac_create_node(g_context, g_op_entry, op_name.c_str());
    nac_graph test_graph = nac_create_graph(g_context);
    nac_graph_attach_node(test_graph, &test_node, 1);

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
    std::cout<<"op entry: ";
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
