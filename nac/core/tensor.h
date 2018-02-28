#ifndef NAC_TENSOR_H
#define NAC_TENSOR_H

#include "common.h"
#include <cstdint>

namespace nac{

using _nac_tensor = class tensor;

class tensor{
public:
    tensor(int _w, int _h, int _c, int _n, void * _ptr, void  (*_del)(void *)=nullptr) :
        w_(_w), h_(_h), c_(_c), n_(n_), data_(_ptr), deleter_(_del) {}
    tensor(int _w, int _h, int _c, int _n, void  (*_del)(void *)=nullptr) : tensor(_w, _h, _c, _n, nullptr, _del){}
    tensor(int _size, void * _ptr, void  (*_del)(void *)=nullptr) :
        w_(_size), h_(1), c_(1), n_(1), data_(_ptr), deleter_(_del) {}
    tensor(void  (*_del)(void *)=nullptr) : tensor(0, 0, 0, 0, _del){}

    virtual ~tensor(){
        if(deleter_)
            deleter_(data());
        else{
            NAC_WARNING("no deleter found on this tensor object(", (uintptr_t)this, "), memory may not be freed");
        }
    }

    inline bool empty() const {return data_==nullptr;}
    inline void feed_data(int _w, int _h, int _c, int _n, void * _ptr){
        NAC_ASSERT(_ptr, "data pointer must not be nullptr")
        w() = _w;
        h() = _h;
        c() = _c;
        n() = _n;
        data() = _ptr;
    }
private:
    void  (*deleter_)(void *);

    NAC_RW_ATTR(void *, data)
    NAC_R_ATTR(int, w)
    NAC_R_ATTR(int, h)
    NAC_R_ATTR(int, c)
    NAC_R_ATTR(int, n)
};

}

#endif