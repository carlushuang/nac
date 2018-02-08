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

    virtual int forward(const tensor ** inputs, tensor * output)
    {
        NAC_ASSERT_COND(inputs, "input tensor should not be NULL");
        NAC_ASSERT_COND(output, "output tensor should not be NULL");

        const conv_hparam * hparam = static_cast<const conv_attr*>(get_hparam());
        // assume one input
        const tensor * input = inputs[0];
        const tensor * weight = get_weights()[0];
        NAC_ASSERT_COND( (input->n() == hparam->batch()) && (output->n() == hparam->batch()), 
            "input/output/op batch size not the same, ", 
            input->n(),"/", output->n(), "/",hparam->batch(), " each");
        NAC_ASSERT_COND( hparam->filters() == weight->n(),
            "hparam filters not same as weight tensor", 
            hparam->filters(), "/", weight->n()," each" );

        float * col_ptr = (float*)request_workspace(sizeof(float) *
            im2col_cpu_outsize(input->c(), input->h(), input->w(), hparam->kernel(), hparam->stride(), hparam->padding()));
        int i;

        int m = hparam->filters();
        int k = hparam->kernel() * hparam->kernel() * input->c();
        int n = output->w() * output->h();
        for(i=0;i<hparam->batch();i++){
            float *a = (float *) weight->data();
            float *b = col_ptr;
            float *c = (float * )output->data() + i*output->w()*output->h()*output->c();

            im2col_cpu((float*)input->data()+i*input->w()*input->h()*input->c(),
                input->c(), input->h(), input->w(), hparam->kernel(), hparam->stride(), hparam->padding(), b);
            gemm_cpu(0,0,m,n,k,1,a,k,b,n,1,c,n);
        }

        if(hparam->act_type() != activation_type::LINEAR)
            activate_cpu(output->data(), output->size(), hparam->act_type());
        
        return 0;
    }
};

NAC_OP_REGISTER(c, NAC_DATA_FP32, conv, op_c_fp32_conv)
}
