#ifndef NAC_OPERATOR_BASE_H
#define NAC_OPERATOR_BASE_H

#include "layer.h"
#include "tensor.h"
#include "context.h"
#include "layer_attribute.h"
#include <string>

namespace nac{

class operator_base {
public:
    operator_base(const char * op_name) : op_name_(op_name){}
    virtual int forward(const tensor ** inputs, tensor * output) = 0;

protected:
    virtual const tensor ** get_weights() const final {
        return layer_->weights;
    }
    virtual int get_num_weights()  const final{
        return layer_->num_weights;
    }
    virtual int get_num_inputs()  const final{
        return layer_->num_inputs;
    }
    virtual const layer_attribute * get_attr()  const final{
        return layer_->attr;
    }
    virtual const context * get_context() const final{
        return layer_->ctx;
    }

private:
    layer * layer_;
    std::string op_name_;
};

}

#endif