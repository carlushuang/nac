#include "op_c_registry.h"
#include "op_c_fp32.h"

#include <operator_base.h>
#include <hyperparameter.h>
#include <tensor.h>
#include <utils.h>


#include <float.h>

namespace nac{

op_c_fp32_maxpool::op_c_fp32_maxpool(const char * op_name) :  operator_base(op_name) {}
op_c_fp32_maxpool::~op_c_fp32_maxpool () {}
int op_c_fp32_maxpool::forward()
{
    const maxpool_hparam * param = static_cast<const maxpool_hparam*>(hparam());
    // assume one x
    const tensor * x = input(0);
    tensor * out = output(0);

    NAC_ASSERT( (x->n() == param->batch()) && (out->n() == param->batch()), 
        "x/out/op batch size not the same, ", 
        x->n(),"/", out->n(), "/",param->batch(), " each");

    int b,i,j,k,m,n;
    int w_offset = -param->padding();
    int h_offset = -param->padding();

    int h = out->h();
    int w = out->w();
    int c = out->c();

    // https://github.com/mratsim/Arraymancer/issues/174
    // direct implementation

    for(b = 0; b < x->n(); ++b){
        for(k = 0; k < c; ++k){
            for(i = 0; i < h; ++i){
                for(j = 0; j < w; ++j){
                    int out_index = j + w*(i + h*(k + c*b));
                    float max = -FLT_MAX;
                    for(n = 0; n < param->kernel(); ++n){
                        for(m = 0; m < param->kernel(); ++m){
                            int cur_h = h_offset + i*param->stride() + n;
                            int cur_w = w_offset + j*param->stride() + m;
                            int index = cur_w + x->w()*(cur_h + x->h()*(k + b*x->c()));
                            int valid = (cur_h >= 0 && cur_h < x->h() &&
                                        cur_w >= 0 && cur_w < x->w());
                            float val = (valid != 0) ? ((float*)x->data())[index] : -FLT_MAX;
                            max   = (val > max) ? val   : max;
                        }
                    }
                    ((float*)out->data())[out_index] = max;
                }
            }
        }
    }

    return 0;
}


NAC_OP_REGISTER(c, NAC_DATA_FP32, maxpool, op_c_fp32_maxpool)

}
