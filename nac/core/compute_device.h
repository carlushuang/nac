#ifndef NAC_COMPUTE_DEVICE_H
#define NAC_COMPUTE_DEVICE_H

#include <string>
#include <vector>
#include "common.h"

namespace nac{

class op_registry;

class compute_device{
public:
    compute_device(op_registry * _op_regi, const char * dev_name = nullptr);
    ~compute_device();

    //int select_op_entry(const char * entry_name);
    //inline const std::string & current_entry_name()const {return current_entry_name_;}

    void init(){}

    void assign_workspace_allocator(void * (*allocator)(int ), void (*deleter)(void *));

    virtual void *  request_workspace(int bytes) final ;
    virtual void clear_workspace() final;

private:
    //std::string                   current_entry_name_;
    //std::vector<operator_base *>  current_ops_;

    void              * workspace_ptr;
    int                 workspace_bytes;

    void * (*workspace_allocator)(int _bytes);
    void (*workspace_deleter)(void * _ptr);

NAC_RW_ATTR(op_registry*, registry)
NAC_R_ATTR(std::string, name)
};

using _nac_device = class compute_device;

void probe_compute_devices(std::vector<compute_device*> & dev_list);
void probe_compute_devices(compute_device** & dev_list, int & num);

}

#endif
