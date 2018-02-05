#ifndef NAC_SHAPE_BASE_H
#define NAC_SHAPE_BASE_H

#include <nac_internal.h>

namespace nac{

// dimension can not change, static array
template < int DIM >
class shape_base{
public:
    shape_base(){
        // default ctor
        for(int i=0;i<DIM;i++)
            dims[i] = 0;
    }
    inline int ndim() const {return DIM;}

protected:
    inline int dim(int i) const {
        NAC_ASSERT_COND(i<DIM, "try to get dim:", i, " exceed total dim:", DIM);
        return dims[i];
    }
    inline int & dim(int i) {
        NAC_ASSERT_COND(i<DIM, "try to get dim:", i, " exceed total dim:", DIM);
        return dims[i];
    }

    int dims[DIM];
};

}

#endif