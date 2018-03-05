#include "op_c_registry.h"

#include <operator_base.h>
#include <hyperparameter.h>
#include <tensor.h>
#include <utils.h>

namespace nac{

class op_armv7_fp32_conv : public operator_base{
public:
    void op_armv7_fp32_conv(const char * op_name) :  operator_base(op_name) {}
    ~op_armv7_fp32_conv () {}

    virtual int forward()
    {
        
        return 0;
    }
};

NAC_OP_REGISTER(armv7, NAC_DATA_FP32, conv, op_armv7_fp32_conv)
}
