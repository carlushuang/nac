#ifndef NAC_OPERATOR_BASE_H
#define NAC_OPERATOR_BASE_H

#include "layer.h"
#include "tensor.h"

namespace nac{

class operator_base {
public:
    virtual int forward(layer & cur_layer, tensor & out_tensor) = 0;
private:

};

}

#endif