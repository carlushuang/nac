#ifndef NAC_COMPUTE_DEVICE_H
#define NAC_COMPUTE_DEVICE_H

#include <string>
#include <vector>

namespace nac{
class op_registry;

using _nac_device = class compute_device;

class compute_device{
public:
    compute_device(op_registry * _op_regi, const char * dev_name = nullptr){
        op_registry() = _op_entries;
        op_registry()->assign_working_device(this);
        if(dev_name)
            name() = dev_name;
        else
            name() = _op_entries->name();

        workspace_allocator = nullptr;
        workspace_deleter = nullptr;
    }
    ~compute_device(){
        clear_workspace();
    }

    //int select_op_entry(const char * entry_name);
    //inline const std::string & current_entry_name()const {return current_entry_name_;}

    void init(){}

    void assign_workspace_allocator(void * (*allocator)(int ), void (*deleter)(void *)){
        workspace_allocator = allocator;
        workspace_deleter = deleter;
    }

    virtual void *  request_workspace(int bytes) final {
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
    virtual void clear_workspace() final{
        if(workspace_deleter){
            workspace_deleter(workspace_ptr);
            return;
        }
        // default cpu based deleter
        if(workspace_ptr){
            delete [] workspace_ptr;
            workspace_bytes = 0;
        }
    }

private:
    //std::string                   current_entry_name_;
    //std::vector<operator_base *>  current_ops_;

    void              * workspace_ptr;
    int                 workspace_bytes;

    void * (*workspace_allocator)(int _bytes);
    void (*workspace_deleter)(void * _ptr);

NAC_RW_ATTR(op_registry *, registry)
NAC_R_ATTR(std::string, name)
};


void probe_compute_devices(std::vector<compute_device*> & dev_list);
void probe_compute_devices(compute_device** & dev_list, int & num);

}

#endif
