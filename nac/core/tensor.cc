#include "tensor.h"
#include "data_mm.h"

namespace nac{


#define WARN_USE_EXTERN_BUFFER()  \
    NAC_WARNING("this tensor ", this, " does not have a data memory manager, please transfer tensor data by your self")

tensor::tensor(int _w, int _h, int _c, int _n, data_mm * _dm) : w_(_w), h_(_h), c_(_c), n_(_n), data_(nullptr), dm_(_dm)
{
    if(!use_external_buffer()){
        int nmemb = w()*h()*c()*n();
        alloc(nmemb);
    }
}

tensor::tensor(int _size, data_mm * _dm) : tensor(_size, 1, 1, 1, _dm) {}

tensor::tensor(data_mm * _dm) : tensor(0, 0, 0, 0, _dm){}

tensor::tensor(const tensor & rhs){
    // TODO: size not same
    //NAC_ASSERT(data_ == nullptr);
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
tensor & tensor::operator=(const tensor & rhs){
    // TODO: size not same
    //NAC_ASSERT(data_ == nullptr);
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

tensor::tensor(tensor && rhs){
    //NAC_ASSERT(data_ == nullptr);
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
tensor & tensor::operator=(tensor && rhs){
    //NAC_ASSERT(data_ == nullptr);
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

tensor::~tensor(){
    free();
}


void tensor::feed_external(void * _ptr){
    return feed_external(w(), h(), c(), n(), _ptr);
}

void tensor::feed_external(int _w, int _h, int _c, int _n, void * _ptr){
    NAC_ASSERT(_ptr, "data pointer must not be nullptr");
    NAC_ASSERT(dm_ == nullptr, "when use external data, data_mm must be nullptr");
    w_ = _w;
    h_ = _h;
    c_ = _c;
    n_ = _n;
    data_ = _ptr;
}

int tensor::copy_to_host(void * host_ptr, int host_offset, int this_offset, int nmemb){
    if(use_external_buffer()){
        WARN_USE_EXTERN_BUFFER();
        return -1;
    }
    return dm()->memcpy_h2d(host_ptr, host_offset, data(), this_offset, nmemb);
}
int tensor::copy_from_host(void * host_ptr, int host_offset, int this_offset, int nmemb){
    if(use_external_buffer()){
        WARN_USE_EXTERN_BUFFER();
        return -1;
    }
    return dm()->memcpy_d2h(data(), this_offset, host_ptr, host_offset, nmemb);
}

int tensor::copy(const tensor & other, int other_offset, int this_offset, int nmemb){
    if(use_external_buffer()){
        WARN_USE_EXTERN_BUFFER();
        return -1;
    }
    return dm()->memcpy_d2d(data(), this_offset, other.data(), other_offset, nmemb);
}
int tensor::move(tensor & other){
    data_ = other.data_;
    other.data_ = nullptr;
    return 0;
}
void * tensor::alloc(int nmemb){
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

void tensor::free(){
    if(dm_ && data_){
        dm()->deleter(data_);
        data_ = nullptr;
    }
}


}