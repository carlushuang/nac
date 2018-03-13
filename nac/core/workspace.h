#ifndef __WORKSPACE_H
#define __WORKSPACE_H

#include "data_mm.h"
#include "common.h"
namespace nac{

class workspace{
public:
    workspace(data_mm * _dm) : dm_(_dm), ptr_(nullptr), bytes_(0){
        NAC_ASSERT(_dm);
    }
    ~workspace(){
        release();
    }

    inline void * request(int bytes){
        if(ptr_){
            if(bytes > bytes_){
                // TODO: realloc
                dm_->deleter(ptr_);
                ptr_ = dm_->allocator(bytes);
                bytes_ = bytes;
            }
        }else{
            ptr_ = dm_->allocator(bytes);
            bytes_ = bytes;
        }
        return ptr_;
    }
    inline void release(){
        if(ptr_)
            dm_->deleter(ptr_);
    }
    data_mm * const & dm()const{return dm_;}
private:
    void      * ptr_;
    int         bytes_;
    data_mm   * dm_;
};

}
#endif