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

    // input, hold only the pointer
    inline void feed_input(tensor * x){
        inputs_.push_back(x);
    }
    inline void feed_inputs(tensor ** x, int num){
        for(int i=0;i<num;i++){
            inputs_.push_back(x[i]);
        }
    }
    inline void feed_inputs(std::vector<tensor*> &x_vec){
        for(auto x : x_vec){
            inputs_.push_back(x);
        }
    }
    inline std::vector<tensor*> inputs() const {
        return inputs_; //copy
    }
    inline const tensor * input(int idx=0) const {
        NAC_ASSERT(idx < inputs_.size(), "request input index:", idx, " bigger than size:", inputs_.size());
        return inputs_.at(idx);
    }

    // weight, we manage the memory for you
    inline void feed_weight(tensor * w){
        weights_.emplace_back(w);
    }
    inline void feed_weights(tensor ** w, int num){
        for(int i=0;i<num;i++){
            weights_.emplace_back(w[i]);
        }
    }
    inline void feed_weights(std::vector<tensor*> &w_vec){
        for(auto w : w_vec){
            weights_.emplace_back(w);
        }
    }
    inline const tensor * weight(int idx=0) const {
        NAC_ASSERT(idx < weights_.size(), "request weight index:", idx, " bigger than size:", weights_.size());
        return weights_.at(idx).get();
    }
    inline std::vector<tensor*> weights const {
        std::vector<tensor*> vec;
        for(auto & ptr : weights_)
            vec.push_back(ptr.get());
        return vec;
    }

    // output, readonly
    inline const tensor * output(int idx = 0) const {
        NAC_ASSERT(idx < outputs_.size(), "request output index:", idx, " bigger than size:", outputs_.size());
        return outputs_.at(idx).get();
    }
    inline std::vector<tensor*> outputs() const {
        std::vector<tensor*> vec;
        for(auto & ptr : outputs_)
            vec.push_back(ptr.get());
        return vec;
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