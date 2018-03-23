#ifndef __OP_C_FP32_H
#define __OP_C_FP32_H

#include <operator_base.h>

namespace nac{

class op_c_fp32_conv : public operator_base{
public:
    op_c_fp32_conv(const char * op_name);
    virtual ~op_c_fp32_conv ();
    virtual int forward();
};

class op_c_fp32_maxpool : public operator_base{
public:
    op_c_fp32_maxpool(const char * op_name);
    virtual ~op_c_fp32_maxpool ();
    virtual int forward();
};
class op_c_fp32_softmax : public operator_base{
public:
    op_c_fp32_softmax(const char * op_name);
    virtual ~op_c_fp32_softmax ();
    virtual int forward();
};

class op_c_fp32_activation : public operator_base{
public:
    op_c_fp32_activation(const char * op_name);
    virtual ~op_c_fp32_activation ();
    virtual int forward();
};

}

#endif