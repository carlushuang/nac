#include <nac.h>
#include <exception>
#include <string>

nac_device      g_device;
nac_context     g_context;

std::string g_test_ops [] = {
    "conv", "maxpool"
};

void prepare(){
    nac_status ret;
    nac_device * devs;
    int num_devs;
    ret = nac_get_devices(&devs, &num_devs);
    if(ret != NAC_SUCCESS)
        std::terminate();
    
    for(int i=0;i<num_devs;i++){
        if(devs[i]->name() == "c")
            break;
    }
    if(i == num_devs){
        std::cout<< "can not find device \"c\" "<<std::endl;
        std::terminate();
    }
    g_device = devs[i];

    g_context = nac_create_context(g_device, 1);
    if(!g_context){
        std::cout<< "fail to create context"<<std::endl;
        std::terminate();
    }

}

void test(){
    for(const std::string & test_op : g_test_ops){
        nac_layer layer = nac_create_layer(g_context, g_device, nullptr, test_op.c_str());
        if(!layer){
            std::cout<< "fail to create layer "<<test_op <<std::endl;
            std::terminate();
        }

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
