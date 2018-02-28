#include "op_c_registry.h"

#include <operator_base.h>
#include <hyperparameter.h>
#include <tensor.h>
#include <utils.h>

namespace nac{

class op_c_fp32_conv : public operator_base{
public:
    void op_c_fp32_conv(const char * op_name) :  operator_base(op_name) {}
    ~op_c_fp32_conv () {}

    virtual int forward()
    {
        const conv_hparam * param = static_cast<const conv_attr*>(hparam());
        const tensor * x = input(0);
        const tensor * filter = weight(0);
        tensor * out = output(0);

        NAC_ASSERT( (x->n() == param->batch()) && (out->n() == param->batch()), 
            "x/out/op batch size not the same, ", 
            x->n(),"/", out->n(), "/",param->batch(), " each");
        NAC_ASSERT( param->filters() == filter->n(),
            "param filters not same as filter tensor", 
            param->filters(), "/", filter->n()," each" );

        float * col_ptr = (float*)request_workspace(sizeof(float) *
            im2col_cpu_outsize(x->c(), x->h(), x->w(), param->kernel(), param->stride(), param->padding()));
        int i;

        int m = param->filters();
        int k = param->kernel() * param->kernel() * x->c();
        int n = out->w() * out->h();
        for(i=0;i<param->batch();i++){
            float *a = (float *) filter->data();
            float *b = col_ptr;
            float *c = (float * )out->data() + i*out->w()*out->h()*out->c();

            im2col_cpu((float*)x->data()+i*x->w()*x->h()*x->c(),
                x->c(), x->h(), x->w(), param->kernel(), param->stride(), param->padding(), b);
            gemm_cpu(0,0,m,n,k,1,a,k,b,n,1,c,n);
        }

        if(param->act_type() != activation_type::LINEAR)
            activate_cpu(out->data(), out->size(), param->act_type());
        
        return 0;
    }
};

NAC_OP_REGISTER(c, NAC_DATA_FP32, conv, op_c_fp32_conv)
}
