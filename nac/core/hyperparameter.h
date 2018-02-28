#ifndef NAC_HYPERPARAMETER_H
#define NAC_HYPERPARAMETER_H

#include "common.h"
#include "node.h"

namespace nac{

class hyperparameter{
public:
    virtual void calculate_outsize(int * out_w, int * out_h, int * out_c, int * out_n) final{
        NAC_ASSERT(op_node() && op_node()->input(), "op_node must be attached, and must have at least one input");
        NAC_ASSERT(out_w && out_h && out_c && out_n, "input param must not be nullptr");
        do_calculate_outsize(out_w, out_h ,out_c ,out_n);
    }
private:
    virtual void do_calculate_outsize(int * out_w, int * out_h, int * out_c, int * out_n) = 0;

NAC_RW_ATTR(int, batch)
NAC_RW_ATTR(node*, op_node)
};

#define  _NAC_INCLUDE_HYPERPARAMETER_DETAIL
#include "hyperparameter/conv_hparam.h"
#include "hyperparameter/activation_hparam.h"
#include "hyperparameter/softmax_hparam.h"
#include "hyperparameter/maxpool_hparam.h"
#undef   _NAC_INCLUDE_HYPERPARAMETER_DETAIL
}

#endif
