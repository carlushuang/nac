#ifndef NAC_COMPUTE_DEVICE_H
#define NAC_COMPUTE_DEVICE_H

#include <string>
#include <vector>

namespace nac{
class op_registry;

using _nac_device = class compute_device;

class compute_device{
public:
    compute_device(op_registry * _op_registed, const char * dev_name = nullptr){
        op_registed_ = _op_registed;
        if(dev_name)
            name_ = dev_name;
        else
            name_ = _op_registed->name();
    }

    inline const std::string & name() const{
        return name_;
    }

    inline op_registry * & op_registed() {return op_registed_;}
    inline const op_registry * & op_registed() const {return op_registed_;}
    int select_op_entry(const char * entry_name);
    inline const std::string & current_entry_name()const {return current_entry_name_;}

    void init(){}
private:
    op_registry *    op_registed_;
    std::string      name_;

    std::string                   current_entry_name_;
    std::vector<operator_base *>  current_ops_;
};


void probe_compute_devices(std::vector<compute_device*> & dev_list);
void probe_compute_devices(compute_device** & dev_list, int & num);

}

#endif
