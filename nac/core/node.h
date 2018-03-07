#ifndef NAC_NODE_H
#define NAC_NODE_H

#include "common.h"
#include "observable.h"
#include <string>
#include <vector>
#include <memory>

namespace nac{

class context;
class tensor;
class hyperparameter;
class operator_base;

class node : public observable{
public:
    //virtual int load_attribute(hyperparameter *);
    //virtual int load_weight(void *);
    node(context * _ctx);
    ~node();

    int forward();

    void attach_op(operator_base*  _op);
    void detach_op();

    void feed_hparam(hyperparameter * _hparam);

    // input, hold only the pointer
    void set_input(tensor * x);
    void set_inputs(tensor ** x, int num);
    void set_inputs(std::vector<tensor*> &x_vec);
    std::vector<tensor*> inputs() const ;
    const tensor * input(int idx=0) const ;

    // weight, we manage the memory for you
    void feed_weight(tensor * w);
    void feed_weights(tensor ** w, int num);
    void feed_weights(std::vector<tensor*> &w_vec);
    const tensor * weight(int idx=0) const ;
    std::vector<tensor*> weights() const ;

    // output, readonly
    tensor * output(int idx = 0) const ;
    std::vector<tensor*> outputs() const ;

protected:
    context             * ctx_;
    operator_base       * op_;
    std::unique_ptr<hyperparameter> hparam_;

    std::vector<tensor*> inputs_;       // not resposible for input memory

    std::vector<std::unique_ptr<tensor>> outputs_;
    std::vector<std::unique_ptr<tensor>> weights_;

    friend class operator_base;

DISABLE_COPY_AND_ASSIGN(node)
};


}

#endif