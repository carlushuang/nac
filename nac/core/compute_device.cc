#include "compute_device.h"
#include <mutex>


namespace nac{

static std::mutex dev_lock;
static std::vector<unique_ptr<compute_device>> dev_list;
static std::vector<compute_device*> dev_ptr_list;   // only a pointer storage place
static std::once_flag  _init_flag;

static inline void init_probe_devices(){
    std::unique_lock<std::mutex> _lock(dev_lock);

    op_registry * default_c_op_regi = get_registry_entry("c");
    dev_list.emplace_back(new compute_device(default_c_op_regi));

    // TODO: dlopen to get dev from shlib


    for(auto & ptr : dev_list){
        dev_ptr_list.push_back(ptr.get());
    }
}

static inline void init_once_probe_devices(){
    try{
        // call_once is safe in concurrent environment
        std::call_once(_init_flag, [](){init_probe_devices();});
    }catch(...){

    }
}

void probe_compute_devices(std::vector<compute_device*> & dev_list){
    init_once_probe_devices();
    for(auto & ptr : dev_list){
        dev_list.push_back(ptr.get());
    }
}
void probe_compute_devices(compute_device** & dev_list, int & num){
    init_once_probe_devices();
    dev_list = dev_ptr_list.data();
    num = dev_ptr_list.size();
}

int compute_device::select_op_entry(const char * entry_name){
    op_registry::op_map_type * entry = op_registed_->get_ops(entry_name);
    if(!entry)
        return -1;
    std::vector<operator_base *> tmp;
    for(auto it = entry->begin();it != entry->end();it++){
        tmp.push_back(it->second.get());
    }
    current_ops_.swap(tmp);
    current_entry_name_ = entry_name;
    return 0;
}

}
