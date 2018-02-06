#ifndef NAC_TENSOR_SHAPE_H
#define NAC_TENSOR_SHAPE_H

#include "shape_base.h"
namespace nac{

// 4d-tensor
class tensor_shape : public shape_base<4>{
public:
    tensor_shape(){}
    tensor_shape(int _w, int _h, int _c)
    {
        dim(0) = _w;
        dim(1) = _h;
        dim(2) = _c;
        dim(3) = 1;
    }
    tensor_shape(int _w, int _h, int _c, int _n)
    {
        dim(0) = _w;
        dim(1) = _h;
        dim(2) = _c;
        dim(3) = _n;
    }

    inline int & w() {return dim(0);}
    inline int w() const {return dim(0);}

    inline int & h() {return dim(1);}
    inline int h() const {return dim(1);}

    inline int & c() {return dim(2);}
    inline int c() const {return dim(2);}

    inline int & n() {return dim(3);}
    inline int n() const {return dim(3);}
};

}

#endif