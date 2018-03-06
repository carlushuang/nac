#include "operator_base.h"
#include "node.h"
#include "tensor.h"
#include "context.h"
#include "hyperparameter.h"
#include "compute_device.h"

namespace nac{

operator_base::operator_base(const char * _name) : full_record_name_(_name), 
            node_(nullptr), dev_(nullptr), ref_cnt(0) 
{}
operator_base::operator_base() : operator_base("N/A") {}
operator_base::~operator_base(){

}


const tensor * operator_base::input(int idx) const {
    NAC_ASSERT(idx<node_->inputs_.size(), "request idx:", idx, " is bigger than input size:", node_->inputs_.size());
    return node_->inputs_.at(idx);
}
const tensor * operator_base::weight(int idx) const  {
    NAC_ASSERT(idx<node_->weights_.size(), "request idx:", idx, " is bigger than weight size:", node_->weights_.size());
    return node_->weights_.at(idx).get();
}
tensor * operator_base::output(int idx)  {
    NAC_ASSERT(idx<node_->outputs_.size(), "request idx:", idx, " is bigger than output size:", node_->outputs_.size());
    return node_->outputs_.at(idx).get();
}

hyperparameter * const operator_base::hparam() const {
    return node_->hparam_.get();
}
context * const operator_base::ctx() const {
    return node_->ctx_;
}

unsigned int operator_base::get() {
    ref_cnt++;
    return ref_cnt.load();
}
unsigned int operator_base::put() {
    ref_cnt--;
    return ref_cnt.load();
}

void *  operator_base::request_workspace(int bytes)  {
    // make sure dev_ has been assigned
    NAC_ASSERT(dev_, "device has not been assigned to this op");
    return dev_->request_workspace(bytes);
}


}



