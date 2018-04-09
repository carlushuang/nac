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

void finish(){
    nac_context_release(g_context);
}

static std::string wrap_blob_file(const char * pure_fname){
    char * var = std::getenv("BLOB_DIR");
    if(!var)
        return pure_fname;
    std::string full_name(var);
    full_name += "/";
    full_name += pure_fname;
    return full_name;
}

void test(){
    std::string cfg_file = wrap_blob_file("yolov3.cfg");
    std::string weight_file = wrap_blob_file("yolov3.weights");
    std::string args_to_brew;
    args_to_brew.append("cfg_file=").append(cfg_file).append(":weight_file=").append(weight_file);
    std::cout<<"++++"<<args_to_brew<<std::endl;
    nac_brew br = nac_brew_create(g_context, args_to_brew.c_str());
    //...
    if(!br){
        std::cout<<"create brew fail"<<std::endl;
        return;
    }

    nac_brew_release(br);
}

int main(int argc, char ** argv) {
    prepare();
    test();
    finish();

    return 0;
}
