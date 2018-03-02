#ifndef NAC_TENSOR_H
#define NAC_TENSOR_H

#include "common.h"
#include "data_mm.h"
#include <cstdint>

namespace nac{

using _nac_tensor = class tensor;

class tensor{
public:
    tensor(int _w, int _h, int _c, int _n, void * _ptr, data_mm * _dm = nullptr) :
        w_(_w), h_(_h), c_(_c), n_(n_), data_(_ptr), dm_(_dm) {}

    tensor(int _w, int _h, int _c, int _n, data_mm * _dm = nullptr) : tensor(_w, _h, _c, _n, nullptr, _dm){}

    tensor(int _size, void * _ptr, data_mm * _dm = nullptr) :
        w_(_size), h_(1), c_(1), n_(1), data_(_ptr), dm_(_dm) {}

    tensor(data_mm * _dm = nullptr) : tensor(0, 0, 0, 0, _dm){}

    virtual ~tensor(){
        if(dm_)
            dm_->deleter(data_);
        else{
            NAC_WARNING("no dm found on this tensor object(", (uintptr_t)this, "), memory may not be freed");
        }
    }

    inline bool empty() const {return data_==nullptr;}
    inline void feed_data(int _w, int _h, int _c, int _n, void * _ptr){
        NAC_ASSERT(_ptr, "data pointer must not be nullptr");
        w_ = _w;
        h_ = _h;
        c_ = _c;
        n_ = _n;
        data_ = _ptr;
    }
private:

    NAC_R_ATTR(void *, data)
    NAC_R_ATTR(int, w)
    NAC_R_ATTR(int, h)
    NAC_R_ATTR(int, c)
    NAC_R_ATTR(int, n)
    NAC_R_ATTR(data_mm*, dm)
};

}

#endif