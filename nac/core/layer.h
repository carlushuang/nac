#ifndef NAC_LAYER_H
#define NAC_LAYER_H

#include "context.h"
#include "operator_base.h"
#include "observable.h"
#include "hyperparameter.h"
#include "tensor.h"

namespace nac{

using _nac_layer = class layer;

class layer : public observable{
public:
    virtual int load_attribute(hyperparameter *);
    virtual int load_weight(void *);

protected:
    context             * ctx;
    operator_base       * op;
    hyperparameter      * hparam;

    // weight tensor
    tensor             ** weights;
    int                   num_weights;

    // input tensor
    tensor             ** inputs;
    int                   num_inputs;

    // output tensor, one output
    tensor              * output;


    friend class operator_base;
};

}

#endif