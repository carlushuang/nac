#ifndef NAC_TENSOR_H
#define NAC_TENSOR_H

#include "common.h"
#include "data_mm.h"
#include <cstdint>

namespace nac{

#define WARN_USE_EXTERN_BUFFER()  \
    NAC_WARNING("this tensor ", this, " does not have a data memory manager, please transfer tensor data by your self")

/*
 * class tensor
 * 
 * nd array of _device_ data.
 * 
 * represent 2 types of tensor:
 * 1) with memory management   (apply _dm in ctor)
 * 2) use external memory buffer  (do not apply _dm in ctor)
 * 
 */
// TODO:
// maybe implement copy-on-write with shared_ptr
class tensor{
public:
    //tensor(int _w, int _h, int _c, int _n, void * _ptr, data_mm * _dm = nullptr) :
    //    w_(_w), h_(_h), c_(_c), n_(_n), data_(_ptr), dm_(_dm) {}

    tensor(int _w, int _h, int _c, int _n, data_mm * _dm = nullptr) : w_(_w), h_(_h), c_(_c), n_(_n), data_(nullptr), dm_(_dm)
    {
        if(!use_external_buffer()){
            int nmemb = w()*h()*c()*n();
            alloc(nmemb);
        }
    }

    tensor(int _size, data_mm * _dm = nullptr) : tensor(_size, 1, 1, 1, _dm) {}

    tensor(data_mm * _dm = nullptr) : tensor(0, 0, 0, 0, _dm){}

    tensor(const tensor & rhs){
        // TODO: size not same
        NAC_ASSERT(data_ == nullptr);
        w_ = rhs.w();
        h_ = rhs.h();
        c_ = rhs.c();
        n_ = rhs.n();
        dm_ = rhs.dm();
        data_ = nullptr;
        if(!use_external_buffer()){
            int nmemb =  w_*h_*c_*n_;
            alloc(nmemb);
            copy(rhs, 0, 0, nmemb);
        }
    }
    tensor & operator=(const tensor & rhs){
        // TODO: size not same
        NAC_ASSERT(data_ == nullptr);
        w_ = rhs.w();
        h_ = rhs.h();
        c_ = rhs.c();
        n_ = rhs.n();
        dm_ = rhs.dm();
        data_ = nullptr;
        if(!use_external_buffer()){
            int nmemb =  w_*h_*c_*n_;
            alloc(nmemb);
            copy(rhs, 0, 0, nmemb);
        }
        return *this;
    }

    tensor(tensor && rhs){
        NAC_ASSERT(data_ == nullptr);
        w_ = rhs.w();
        h_ = rhs.h();
        c_ = rhs.c();
        n_ = rhs.n();
        dm_ = rhs.dm();
        data_ = nullptr;
        if(!use_external_buffer()){
            int nmemb =  w_*h_*c_*n_;
            alloc(nmemb);
            move(rhs);
        }
    }
    tensor & operator=(tensor && rhs){
        NAC_ASSERT(data_ == nullptr);
        w_ = rhs.w();
        h_ = rhs.h();
        c_ = rhs.c();
        n_ = rhs.n();
        dm_ = rhs.dm();
        data_ = nullptr;
        if(!use_external_buffer()){
            int nmemb =  w_*h_*c_*n_;
            alloc(nmemb);
            move(rhs);
        }
        return *this;
    }

    virtual ~tensor(){
        free();
    }

    inline bool empty() const {return data_==nullptr;}

    inline void feed_external(void * _ptr){
        return feed_external(w(), h(), c(), n(), _ptr);
    }

    inline void feed_external(int _w, int _h, int _c, int _n, void * _ptr){
        NAC_ASSERT(_ptr, "data pointer must not be nullptr");
        NAC_ASSERT(dm_ == nullptr, "when use external data, data_mm must be nullptr");
        w_ = _w;
        h_ = _h;
        c_ = _c;
        n_ = _n;
        data_ = _ptr;
    }

    inline int copy_to_host(void * host_ptr, int host_offset, int this_offset, int nmemb){
        if(use_external_buffer()){
            WARN_USE_EXTERN_BUFFER();
            return -1;
        }
        return dm()->memcpy_h2d(host_ptr, host_offset, data(), this_offset, nmemb);
    }
    inline int copy_from_host(void * host_ptr, int host_offset, int this_offset, int nmemb){
        if(use_external_buffer()){
            WARN_USE_EXTERN_BUFFER();
            return -1;
        }
        return dm()->memcpy_d2h(data(), this_offset, host_ptr, host_offset, nmemb);
    }

    inline int copy(const tensor & other, int other_offset, int this_offset, int nmemb){
        if(use_external_buffer()){
            WARN_USE_EXTERN_BUFFER();
            return -1;
        }
        return dm()->memcpy_d2d(data(), this_offset, other.data(), other_offset, nmemb);
    }
    inline int move(tensor & other){
        data_ = other.data_;
        other.data_ = nullptr;
        return 0;
    }
    inline void * alloc(int nmemb){
        if(use_external_buffer()){
            WARN_USE_EXTERN_BUFFER();
            return nullptr;
        }
        if(data_){
            // TODO: realloc
            NAC_WARNING("have alloc before");
            return nullptr;
        }
        data_ = dm()->allocator(nmemb);
        NAC_ASSERT(data_);
        return data_;
    }

    inline void free(){
        if(dm_ && data_){
            dm()->deleter(data_);
            data_ = nullptr;
        }
    }

private:

    inline bool use_external_buffer() {return dm()==nullptr;}

    NAC_R_ATTR(void *, data)
    NAC_R_ATTR(int, w)
    NAC_R_ATTR(int, h)
    NAC_R_ATTR(int, c)
    NAC_R_ATTR(int, n)
    NAC_R_ATTR(data_mm*, dm)
};

using _nac_tensor = tensor;

}

#endif