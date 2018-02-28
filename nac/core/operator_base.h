#ifndef NAC_OPERATOR_BASE_H
#define NAC_OPERATOR_BASE_H

#include "node.h"
#include "tensor.h"
#include "context.h"
#include "hyperparameter.h"
#include <string>
#include <atomic>

namespace nac{

using _nac_operator = class operator_base;

class operator_base {
public:
    operator_base(const char * _name) : full_record_name(_name), ref_cnt(0) {
        workspace=nullptr;
        workspace_bytes = 0;
        layer_ = nullptr;
        dev_ = nullptr;
    }
    virtual ~operator_base(){

    }

    virtual int forward() = 0;
    const std::string & name(){return full_record_name;}

    virtual node * & working_node() final {return node_;}
    virtual compute_device * & working_device() final {return dev_;}

protected:
    virtual const tensor * input(int idx) const final {
        NAC_ASSERT(idx<node_->inputs_.size(), "request idx:", idx, " is bigger than input size:", node_->inputs_.size());
        return node_->inputs_.at(idx);
    }
    virtual const tensor * weight(int idx) const final {
        NAC_ASSERT(idx<node_->weights_.size(), "request idx:", idx, " is bigger than weight size:", node_->weights_.size());
        return node_->weights_.at(idx).get();
    }
    virtual tensor * output(int idx) final {
        NAC_ASSERT(idx<node_->outputs_.size(), "request idx:", idx, " is bigger than output size:", node_->outputs_.size());
        return node_->outputs_.at(idx).get();
    }

    virtual const hyperparameter * hparam() const final{
        return node_->hparam_;
    }
    virtual const context * context() const final{
        return node_->ctx_;
    }
    //virtual context * get_context() final{
    //    return node_->ctx;
    //}
    virtual unsigned int get() final{
        ref_cnt++;
        return ref_cnt.load();
    }
    virtual unsigned int put() final{
        ref_cnt--;
        return ref_cnt.load();
    }

    virtual void *  request_workspace(int bytes) final {
        // make sure dev_ has been assigned
        NAC_ASSERT(dev_, "device has not been assigned to this op");
        return dev_->request_workspace(bytes);
    }

private:
    node               * node_;
    compute_device     * dev_;
    std::string          full_record_name;

    std::atomic_uint    ref_cnt;

DISABLE_COPY_AND_ASSIGN(operator_base)
};

}

#endif