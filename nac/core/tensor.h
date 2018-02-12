#ifndef NAC_TENSOR_H
#define NAC_TENSOR_H

#include "common.h"

namespace nac{

using _nac_tensor = class tensor;

class tensor{
public:
    tensor(int _w, int _h, int _c, int _n, void * _ptr) : w_(_w), h_(_h), c_(_c), n_(n_), data_(_ptr) {}
    tensor(int _w, int _h, int _c, int _n) : tensor(_w, _h, _c, _n, nullptr){}
    tensor(int _size, void * _ptr) : w_(_size), h_(1), c_(1), n_(1), data_(_ptr){}
    tensor() : tensor(0, 0, 0, 0){}

    inline bool empty() const {return data_==nullptr;}

    NAC_RW_ATTR(void *, data)
    NAC_RW_ATTR(int, w)
    NAC_RW_ATTR(int, h)
    NAC_RW_ATTR(int, c)
    NAC_RW_ATTR(int, n)
};

}

#endif