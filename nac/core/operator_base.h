#ifndef NAC_OPERATOR_BASE_H
#define NAC_OPERATOR_BASE_H

#include "layer.h"
#include "tensor.h"
#include "context.h"
#include "hyperparameter.h"
#include <string>
#include <atomic>

namespace nac{

class operator_base {
public:
    operator_base(const char * op_name) : op_name_(op_name), ref_cnt(0) {
        workspace=nullptr;
        workspace_bytes = 0;
    }
    ~operator_base(){
        if(workspace)
            delete [] workspace;
    }
    virtual int forward(const tensor ** inputs, tensor * output) = 0;

protected:
    virtual const tensor ** get_weights() const final {
        return layer_->weights;
    }
    virtual int get_num_weights()  const final{
        return layer_->num_weights;
    }
    virtual int get_num_inputs()  const final{
        return layer_->num_inputs;
    }
    virtual const hyperparameter * get_hparam()  const final{
        return layer_->hparam;
    }
    virtual const context * get_context() const final{
        return layer_->ctx;
    }

    virtual void *  request_workspace(int bytes) final {
        if(!workspace){
            workspace = new unsigned char [bytes];
            workspace_bytes = bytes;
        }
        else{
            if(bytes>workspace_bytes){
                delete [] workspace;
                workspace = new unsigned char [bytes];
                workspace_bytes = bytes;
            }
        }
        return static_cast<void*>(workspace);
    }
    virtual void clear_worksapce() final{
        if(workspace){
            delete [] workspace;
            workspace_bytes = 0;
        }
    }

private:
    layer              * layer_;
    std::string          op_name_;

    unsigned char *     workspace;
    int                 workspace_bytes;
    std::atomic_uint    ref_cnt;
};

}

#endif