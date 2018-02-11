#ifndef NAC_COMPUTE_DEVICE_H
#define NAC_COMPUTE_DEVICE_H

#include <string>

namespace nac{
class op_registry;

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

    void init(){}
private:
    op_registry *    op_registed_;
    std::string      name_;
};

}

#endif