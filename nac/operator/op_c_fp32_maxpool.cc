#include "op_c_registry.h"

#include <operator_base.h>
#include <hyperparameter.h>
#include <tensor.h>
#include <utils.h>

namespace nac{

class op_c_fp32_maxpool : public operator_base{
public:
    void op_c_fp32_maxpool(const char * op_name) :  operator_base(op_name) {}
    ~op_c_fp32_maxpool () {}
    virtual int forward(const tensor ** inputs, tensor * output)
    {
        NAC_ASSERT_COND(inputs, "input tensor should not be NULL");
        NAC_ASSERT_COND(output, "output tensor should not be NULL");
        const maxpool_hparam * hparam = static_cast<const maxpool_hparam*>(get_hparam());
        // assume one input
        const tensor * input = inputs[0];

        NAC_ASSERT_COND( (input->n() == hparam->batch()) && (output->n() == hparam->batch()), 
            "input/output/op batch size not the same, ", 
            input->n(),"/", output->n(), "/",hparam->batch(), " each");

        int b,i,j,k,m,n;
        int w_offset = -hparam->padding();
        int h_offset = -hparam->padding();

        int h = output->h();
        int w = output->w();
        int c = output->c();

        // https://github.com/mratsim/Arraymancer/issues/174
        // direct implementation

        for(b = 0; b < input->n(); ++b){
            for(k = 0; k < c; ++k){
                for(i = 0; i < h; ++i){
                    for(j = 0; j < w; ++j){
                        int out_index = j + w*(i + h*(k + c*b));
                        float max = -FLT_MAX;
                        for(n = 0; n < hparam->kernel(); ++n){
                            for(m = 0; m < hparam->kernel(); ++m){
                                int cur_h = h_offset + i*hparam->stride() + n;
                                int cur_w = w_offset + j*hparam->stride() + m;
                                int index = cur_w + input->w()*(cur_h + input->h()*(k + b*input->c()));
                                int valid = (cur_h >= 0 && cur_h < input->h() &&
                                            cur_w >= 0 && cur_w < input->w());
                                float val = (valid != 0) ? ((float*)input->data())[index] : -FLT_MAX;
                                max   = (val > max) ? val   : max;
                            }
                        }
                        ((float*)output->data())[out_index] = max;
                    }
                }
            }
        }
    }
};

NAC_OP_REGISTER(c, NAC_DATA_FP32, maxpool, op_c_fp32_maxpool)

}
