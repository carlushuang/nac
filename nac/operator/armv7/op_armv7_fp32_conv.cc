#include "op_armv7_registry.h"
#include <op_c_fp32.h>

#include <operator_base.h>
#include <hyperparameter.h>
#include <tensor.h>
#include <utils.h>

namespace nac{

class op_armv7_fp32_conv : public op_c_fp32_conv{
public:
    op_armv7_fp32_conv(const char * op_name) :  op_c_fp32_conv(op_name) {}
    ~op_armv7_fp32_conv () {}

    virtual int forward()
    {
        return op_c_fp32_conv::forward();
    }
};

NAC_OP_REGISTER(armv7, NAC_DATA_FP32, conv, op_armv7_fp32_conv)
}
