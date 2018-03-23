#include "compute_device.h"
#include "op_registry.h"
#include <mutex>
#include <memory>
#include <vector>
#include <string>

namespace nac{

static std::mutex g_dev_lock;
static std::vector<std::unique_ptr<compute_device>> g_dev_list;
static std::vector<compute_device*> g_dev_ptr_list;   // only a pointer storage place
static std::once_flag  g_init_flag;


static std::vector<std::string> get_library_path(){
#ifdef __linux__
    std::vector<std::string> lib_path = {"/usr/lib", "/lib", "/system/lib"};
#else
    std::vector<std::string> tmp
    return tmp;
#endif
}

static inline void dynamic_load_devices(){
#ifdef __linux__
    static const char * supported_device[] = {}
#endif
}

static inline void init_probe_devices(){
    std::unique_lock<std::mutex> _lock(g_dev_lock);

    op_registry * default_c_op_regi = get_registry_entry("c");
    compute_device * default_c_device = new compute_device(default_c_op_regi);
    //default_c_device->select_op_entry("fp32");
    g_dev_list.emplace_back(default_c_device);

    // TODO: dlopen to get dev from shlib
    for(auto & ptr : g_dev_list){
        g_dev_ptr_list.push_back(ptr.get());
    }
}

static inline void init_once_probe_devices(){
    try{
        // call_once is safe in concurrent environment
        std::call_once(g_init_flag, [](){init_probe_devices();});
    }catch(const std::exception& e){
        NAC_ERROR("init device error! ", e.what());
        NAC_ABORT();
    }
}

void probe_compute_devices(std::vector<compute_device*> & dev_list){
    init_once_probe_devices();
    for(auto & ptr : g_dev_list){
        dev_list.push_back(ptr.get());
    }
}
void probe_compute_devices(compute_device*** dev_list, int * num){
    init_once_probe_devices();
    *dev_list = g_dev_ptr_list.data();
    *num = g_dev_ptr_list.size();
}

compute_device::compute_device(op_registry * _op_regi, const char * dev_name ){
    registry_ = _op_regi;
    registry_->assign_working_device(this);
    if(dev_name)
        name_ = dev_name;
    else
        name_ = _op_regi->name();
}

compute_device::~compute_device(){

}

}
