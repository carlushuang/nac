#ifndef NAC_TENSOR_ATTRIBUTE_H
#define NAC_TENSOR_ATTRIBUTE_H
#include "attribute.h"

namespace nac{

class tensor_attribute : public attribute{
public:
    tensor_attribute(int _w, int _h, int _c) :
        w(_w), h(_h), c(_c), n(1)
    {}
    tensor_attribute(int _w, int _h, int _c, int _n) :
        w(_w), h(_h), c(_c), n(_n)
    {}

    int & width() {return this->w;}
    int width() const {return this->w;}

    int & height() {return this->h;}
    int height() const {return this->h;}

    int & channel() {return this->c;}
    int channel() const {return this->c;}

    int & num() {return this->n;}
    int num() const {return this->n;}
private:
    int w,h,c,n;
};

}

#endif