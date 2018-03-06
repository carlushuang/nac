#include "compute_device.h"
#include "op_registry.h"
#include <mutex>
#include <memory>

namespace nac{

static std::mutex g_dev_lock;
static std::vector<std::unique_ptr<compute_device>> g_dev_list;
static std::vector<compute_device*> g_dev_ptr_list;   // only a pointer storage place
static std::once_flag  g_init_flag;

static inline void dynamic_load_devices(){
    
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
    }catch(...){

    }
}

void probe_compute_devices(std::vector<compute_device*> & dev_list){
    init_once_probe_devices();
    for(auto & ptr : g_dev_list){
        dev_list.push_back(ptr.get());
    }
}
void probe_compute_devices(compute_device** & dev_list, int & num){
    init_once_probe_devices();
    dev_list = g_dev_ptr_list.data();
    num = g_dev_ptr_list.size();
}
#if 0
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
#endif



compute_device::compute_device(op_registry * _op_regi, const char * dev_name ){
    registry_ = _op_regi;
    registry_->assign_working_device(this);
    if(dev_name)
        name_ = dev_name;
    else
        name_ = _op_regi->name();

    workspace_allocator = nullptr;
    workspace_deleter = nullptr;
}

compute_device::~compute_device(){
    clear_workspace();
}

void compute_device::assign_workspace_allocator(void * (*allocator)(int ), void (*deleter)(void *)){
    workspace_allocator = allocator;
    workspace_deleter = deleter;
}

void * compute_device::request_workspace(int bytes) {
    if(workspace_allocator){
        workspace_ptr = workspace_allocator(bytes);
        workspace_bytes = bytes;
        return workspace_ptr;
    }
    // default cpu based allocator
    if(!workspace_ptr){
        workspace_ptr = static_cast<void*>(new unsigned char [bytes]);
        workspace_bytes = bytes;
    }
    else{
        if(bytes>workspace_bytes){
            delete [] ((unsigned char *)workspace_ptr);
            workspace_ptr = static_cast<void*>(new unsigned char [bytes]);
            workspace_bytes = bytes;
        }
    }
    return workspace_ptr;
}
void compute_device::clear_workspace() {
    if(workspace_deleter){
        workspace_deleter(workspace_ptr);
        return;
    }
    // default cpu based deleter
    if(workspace_ptr){
        delete [] (unsigned char*)workspace_ptr;
        workspace_bytes = 0;
    }
}

}
