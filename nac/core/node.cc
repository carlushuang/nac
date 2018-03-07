#include "context.h"
#include "operator_base.h"

#include "hyperparameter.h"
#include "tensor.h"

namespace nac{


node::node(context * _ctx):ctx_(_ctx),op_(nullptr),hparam_(nullptr)
{}

node::~node(){
    detach_op();
}

int node::forward(){
    op_->working_node() = this;
    return op_->forward();
}

void node::attach_op(operator_base*  _op){
    op_ = _op;
    op_->get();
}
void node::detach_op(){
    op_->put();
}

void node::feed_hparam(hyperparameter * _hparam){
    hparam_.reset(_hparam);
}

// input, hold only the pointer
void node::set_input(tensor * x){
    inputs_.push_back(x);
}
void node::set_inputs(tensor ** x, int num){
    for(int i=0;i<num;i++){
        inputs_.push_back(x[i]);
    }
}
void node::set_inputs(std::vector<tensor*> &x_vec){
    for(auto x : x_vec){
        inputs_.push_back(x);
    }
}
std::vector<tensor*> node::inputs() const {
    return inputs_; //copy
}
const tensor * node::input(int idx) const {
    NAC_ASSERT(idx < inputs_.size(), "request input index:", idx, " bigger than size:", inputs_.size());
    return inputs_.at(idx);
}

// weight, we manage the memory for you
void node::feed_weight(tensor * w){
    weights_.emplace_back(w);
}
void node::feed_weights(tensor ** w, int num){
    for(int i=0;i<num;i++){
        weights_.emplace_back(w[i]);
    }
}
void node::feed_weights(std::vector<tensor*> &w_vec){
    for(auto w : w_vec){
        weights_.emplace_back(w);
    }
}
const tensor * node::weight(int idx) const {
    NAC_ASSERT(idx < weights_.size(), "request weight index:", idx, " bigger than size:", weights_.size());
    return weights_.at(idx).get();
}
std::vector<tensor*> node::weights() const {
    std::vector<tensor*> vec;
    for(auto & ptr : weights_)
        vec.push_back(ptr.get());
    return vec;
}

// output, readonly
tensor * node::output(int idx) const {
    NAC_ASSERT(idx < outputs_.size(), "request output index:", idx, " bigger than size:", outputs_.size());
    return outputs_.at(idx).get();
}
std::vector<tensor*> node::outputs() const {
    std::vector<tensor*> vec;
    for(auto & ptr : outputs_)
        vec.push_back(ptr.get());
    return vec;
}


}