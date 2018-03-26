#include "compute_device.h"
#include "op_registry.h"
#include <mutex>
#include <memory>
#include <vector>
#include <string>

#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <dlfcn.h>
#endif

namespace nac{

static std::mutex g_dev_lock;
static std::vector<std::unique_ptr<compute_device>> g_dev_list;
static std::vector<compute_device*> g_dev_ptr_list;   // only a pointer storage place
static std::once_flag  g_init_flag;


struct plugin{
    std::string name;
    std::string full_path;
    void * handle;
};
static inline std::string get_hw_name(const std::string lib_name){
#ifdef __linux__
    // libnac.hw.<name>.so
    return lib_name.substr(10, lib_name.length()-13);  // TODO: valid
#endif
}

static std::vector<std::string> get_library_path(){
#ifdef __linux__
    std::vector<std::string> lib_path = {"/usr/lib/", "/lib/", "/system/lib/", "/usr/share/lib/"};
    return lib_path;
#else
    std::vector<std::string> tmp
    return tmp;
#endif
}

static inline bool get_plugin_names(const std::string path_name, std::vector<std::string> & plugin_names ){
#ifdef __linux__
    struct dirent * dir;
    DIR * p_dir;
    int any_plugin = 0;

    p_dir = opendir(path_name.c_str());
    if(p_dir == NULL){
        NAC_WARNING("can not open dir to list plugins ", path_name);
        return false;
    }
    while( (dir = readdir(p_dir)) != NULL){
        if(0 == strncmp(dir->d_name, "libnac.hw.", 10)){
            plugin_names.push_back(dir->d_name);
            any_plugin++;
        }
    }
    closedir(p_dir);
    return any_plugin>0;
#endif 
}

static inline void load_hw_plugin(){
#ifdef __linux__
#define SELF_LIB_NAME "libnac.so"
    // assume we have the privilege to access the lib path.
    // this may not true in android environment
    std::vector<std::string> lib_path = get_library_path();
    std::string path;
    std::vector<std::string> plugin_names;
    bool found = false;
    for(auto & str : lib_path){
        struct stat st;
        std::string s = str + SELF_LIB_NAME;
        plugin_names.clear();
        if(0 == stat(s.c_str(), &st)){
            if(get_plugin_names(str, plugin_names)){
                path = str;
                found = true;
                break;
            }
        }
    }
    if(!found){
        NAC_WARNING("no hw plugin found on this system, may use none");
        return ;
    }

    // try get symbol
    for(auto & s : plugin_names){
        std::string plugin_full_name = path+s;
        void * handle = dlopen(plugin_full_name.c_str(), RTLD_NOW);
        if(!handle){
            NAC_ERROR("fail to open plugin ", plugin_full_name);
            continue;
        }
        std::string hw_name = get_hw_name(s);
        std::string regi_symbol = "get_registry_" + hw_name;
        op_registry * (*get_registry_hw)();
        get_registry_hw = reinterpret_cast<decltype(get_registry_hw)>(dlsym(handle, regi_symbol.c_str()));
        if(!get_registry_hw){
            NAC_ERROR("fail to find symbol:", regi_symbol, " for ", hw_name);
            continue;
        }

        op_registry * op_regi_hw = get_registry_hw();
        compute_device * hw_device = new compute_device(op_regi_hw);
        hw_device->plugin_handler() = handle;     // let this device to do the dealloc
        g_dev_list.emplace_back(hw_device);
    }
#endif
}

static inline void init_probe_devices(){
    std::unique_lock<std::mutex> _lock(g_dev_lock);

    load_hw_plugin();

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

compute_device::compute_device(op_registry * _op_regi, const char * dev_name ) : plugin_handler_(nullptr){
    registry_ = _op_regi;
    registry_->assign_working_device(this);
    if(dev_name)
        name_ = dev_name;
    else
        name_ = _op_regi->name();
}

compute_device::~compute_device(){
    if(plugin_handler_){
#ifdef __linux__
        dlclose(plugin_handler_);
#endif
    }
}

}
