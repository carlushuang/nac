#ifndef NAC_OPERATOR_BASE_H
#define NAC_OPERATOR_BASE_H

#include "common.h"
#include <string>
#include <atomic>

namespace nac{

class node;
class compute_device;
class tensor;
class hyperparameter;
class context;

class operator_base {
public:
    
    operator_base(const char * _name);
    operator_base();
    virtual ~operator_base();

    virtual int forward() = 0;
    const std::string & name(){return full_record_name_;}

    virtual node * & working_node() final {return node_;}
    virtual compute_device * & working_device() final {return dev_;}

protected:
    virtual const tensor * input(int idx) const final;
    virtual const tensor * weight(int idx) const final;
    virtual tensor * output(int idx) final;

    virtual hyperparameter * const hparam() const final;
    virtual context * const ctx() const final;
    //virtual context * get_context() final{
    //    return node_->ctx;
    //}
    virtual unsigned int get() final;
    virtual unsigned int put() final;
    virtual void *  request_workspace(int bytes) final ;

private:
    node               * node_;
    compute_device     * dev_;
    std::string          full_record_name_;

    std::atomic_uint    ref_cnt;


friend struct op_register;

DISABLE_COPY_AND_ASSIGN(operator_base)
};

using _nac_operator = operator_base;

}

#endif