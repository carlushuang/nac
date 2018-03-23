#include "op_c_registry.h"
#include "op_c_fp32.h"

#include <operator_base.h>
#include <hyperparameter.h>
#include <tensor.h>
#include <utils.h>

namespace nac{

op_c_fp32_activation::op_c_fp32_activation(const char * op_name) :  operator_base(op_name) {}
op_c_fp32_activation::~op_c_fp32_activation () {}

int op_c_fp32_activation::forward()
{
    // point wise activation
    const activation_hparam * param = static_cast<const activation_hparam*>(hparam());
    const tensor * x = input(0);
    tensor * out = output(0);
    utils::activate_cpu((float*) x->data(), (float*) out->data(), out->size(), param->act_type());
    return 0;
}

NAC_OP_REGISTER(c, NAC_DATA_FP32, activation, op_c_fp32_activation)
}