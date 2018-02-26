#ifndef NAC_OPERATOR_BASE_H
#define NAC_OPERATOR_BASE_H

#include "layer.h"
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
    ~operator_base(){

    }
    virtual int forward(const tensor ** inputs, tensor * output) = 0;
    const std::string & name(){return full_record_name;}

    virtual layer * & working_layer() final {return layer_;}
    virtual compute_device * & working_device() final {return dev_;}

protected:
    virtual const tensor ** get_weights() const final {
        return layer_->weights;
    }
    virtual int get_num_weights() const final{
        return layer_->num_weights;
    }
    virtual int get_num_inputs()  const final{
        return layer_->num_inputs;
    }
    virtual const hyperparameter * get_hparam() const final{
        return layer_->hparam;
    }
    virtual const context * get_context() const final{
        return layer_->ctx;
    }
    virtual context * get_context() final{
        return layer_->ctx;
    }

    virtual void *  request_workspace(int bytes) final {
        // make sure dev_ has been assigned
        NAC_ASSERT_COND(dev_, "device has not been assigned to this op");
        return dev_->request_workspace(bytes);
    }

private:
    layer              * layer_;
    compute_device     * dev_;
    std::string          full_record_name;

    std::atomic_uint    ref_cnt;
};

}

#endif