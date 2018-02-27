#ifndef NAC_NODE_H
#define NAC_NODE_H

#include "context.h"
#include "operator_base.h"
#include "observable.h"
#include "hyperparameter.h"
#include "tensor.h"
#include <string>
#include <vector>

namespace nac{

using _nac_node = class node;

class node : public observable{
public:
    //virtual int load_attribute(hyperparameter *);
    //virtual int load_weight(void *);
    node(context * _ctx):ctx_(_ctx),op_(nullptr),hparam_(nullptr)
        {}

    ~node(){
        detach_op();
    }

    inline int forward(){
        op_->working_node() = this;
        return op_->forward();
    }

    inline void attach_op(operator_base*  _op){
        op_ = _op;
        op_->get();
    }
    inline detach_op(){
        op_->put();
    }

    inline void set_hparam(hyperparameter * _hparam){
        hparam_ = _hparam;
    }


protected:
    context             * ctx_;
    operator_base       * op_;
    hyperparameter      * hparam_;

    std::vector<tensor*> inputs_;       // not resposible for input memory

    std::vector<unique_ptr<tensor>> outputs_;
    std::vector<unique_ptr<tensor>> weights_;

    friend class operator_base;

DISABLE_COPY_AND_ASSIGN(node)
};

}

#endif