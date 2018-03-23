#include "op_c_registry.h"
#include "op_c_fp32.h"

#include <operator_base.h>
#include <hyperparameter.h>
#include <tensor.h>
#include <utils.h>

namespace nac{

op_c_fp32_softmax::op_c_fp32_softmax(const char * op_name) :  operator_base(op_name) {}
op_c_fp32_softmax::~op_c_fp32_softmax () {}
int op_c_fp32_softmax::forward()
{
    // for cache friedly, we may ignore stride/group in darknet concept
    // consider all is row major, can lead to fast performance.
    return 0;
}

NAC_OP_REGISTER(c, NAC_DATA_FP32, softmax, op_c_fp32_softmax)

}
