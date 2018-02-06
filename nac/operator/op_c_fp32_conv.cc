#include "op_c_registry.h"

#include <operator_base.h>
#include <layer_attribute.h>
#include <tensor.h>

namespace nac{

class op_c_fp32_conv : public operator_base{
public:
    void op_c_fp32_conv(const char * op_name) :  operator_base(op_name) {}
    virtual int forward(const tensor ** inputs, tensor * output){
        NAC_ASSERT_COND(inputs, "input tensor should not be NULL");
        NAC_ASSERT_COND(output, "output tensor should not be NULL");
        const conv_attr * attr = static_cast<const conv_attr*>(get_attr());
    }
};

NAC_OP_REGISTER(c, NAC_DATA_FP32, conv, op_c_fp32_conv)
}
