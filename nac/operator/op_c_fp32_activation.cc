#include "op_c_registry.h"

#include <operator_base.h>
#include <hyperparameter.h>
#include <tensor.h>
#include <utils.h>


namespace nac{

class op_c_fp32_activation : public operator_base{
public:
    op_c_fp32_activation(const char * op_name) :  operator_base(op_name) {}
    ~op_c_fp32_activation () {}

    virtual int forward()
    {
        // point wise activation
        const activation_hparam * param = static_cast<const activation_hparam*>(hparam());
        const tensor * x = input(0);
        tensor * out = output(0);
        utils::activate_cpu((float*) x->data(), (float*) out->data(), out->size(), param->act_type());
        return 0;
    }
};

NAC_OP_REGISTER(c, NAC_DATA_FP32, activation, op_c_fp32_activation)
}