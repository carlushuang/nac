#include "context.h"
#include "operator_base.h"

#include "hyperparameter.h"
#include "tensor.h"

namespace nac{


node::node(context * _ctx):ctx_(_ctx),op_(nullptr),hparam_(nullptr),cached_dm(nullptr)
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
    _hparam->op_node() = this;
}

// input, hold only the pointer
void node::set_input(tensor * x){
    valid_dm(x);
    inputs_.push_back(x);
}
void node::set_inputs(tensor ** x, int num){
    for(int i=0;i<num;i++){
        valid_dm(x[i]);
        inputs_.push_back(x[i]);
    }
}
void node::set_inputs(std::vector<tensor*> &x_vec){
    for(auto x : x_vec){
        valid_dm(x);
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

void node::valid_dm(tensor * _t){
    if(!cached_dm)
        cached_dm = _t->dm();
    else{
        NAC_ASSERT(cached_dm == _t->dm(), "dm must be the same");
    }
}

void node::prepare(){
    // TODO: multi prepare
    NAC_ASSERT(cached_dm!=nullptr, "no data_mm found, prepare fail. maybe you need feed input/weight first?");
    int out_w, out_h, out_c, out_n;
    hparam_->calculate_outsize(&out_w, &out_h, &out_c, &out_n);
    outputs_.emplace_back(new tensor(out_w, out_h, out_c, out_n, cached_dm));

}

// weight, we manage the memory for you
void node::feed_weight(tensor * w){
    valid_dm(w);
    weights_.emplace_back(w);
}
void node::feed_weights(tensor ** w, int num){
    for(int i=0;i<num;i++){
        valid_dm(w[i]);
        weights_.emplace_back(w[i]);
    }
}
void node::feed_weights(std::vector<tensor*> &w_vec){
    for(auto w : w_vec){
        valid_dm(w);
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