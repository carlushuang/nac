#include "op_c_registry.h"

#include <operator_base.h>
#include <hyperparameter.h>
#include <tensor.h>
#include <utils.h>


namespace nac{

class op_c_fp32_activation : public operator_base{
public:
    void op_c_fp32_activation(const char * op_name) :  operator_base(op_name) {}
    ~op_c_fp32_activation () {}

    virtual int forward(const tensor ** inputs, tensor * output)
    {
        const activation_hparam * hparam = static_cast<const activation_attr*>(get_hparam());
        activate_cpu((float*) input->data(), (float*) output->data(), output->size(), hparam->act_type());
    }
};

NAC_OP_REGISTER(c, NAC_DATA_FP32, activation, op_c_fp32_activation)
}