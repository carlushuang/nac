#ifndef NAC_LAYER_H
#define NAC_LAYER_H

#include "context.h"
#include "operator_base.h"
#include "observable.h"
#include "hyperparameter.h"
#include "tensor.h"
#include <string>

namespace nac{

using _nac_layer = class layer;

class layer : public observable{
public:
    virtual int load_attribute(hyperparameter *);
    virtual int load_weight(void *);
    layer(context * _ctx):ctx(_ctx),op(nullptr),hparam(nullptr),weights(nullptr),
                    num_weights(0),inputs(nullptr),num_inputs(0),output(nullptr)
        {}
    ~layer(){
    }

    int forward(const tensor ** inputs, tensor * output){
        op->working_layer() = this;
        op->forward(inputs, output);
    }

    void attach_op(operator_base*  _op){
        op = _op;
    }

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