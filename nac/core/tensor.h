#ifndef NAC_TENSOR_H
#define NAC_TENSOR_H

#include "common.h"
#include <cstdint>

namespace nac{

class data_mm;

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

    tensor(int _w, int _h, int _c, int _n, data_mm * _dm = nullptr);
    tensor(int _size, data_mm * _dm = nullptr);
    tensor(data_mm * _dm = nullptr);

    tensor(const tensor & rhs);
    tensor & operator=(const tensor & rhs);

    tensor(tensor && rhs);
    tensor & operator=(tensor && rhs);

    virtual ~tensor();

    inline bool empty() const {return data_==nullptr;}
    inline int size() const {return w()*h()*c()*n();}

    void feed_external(void * _ptr);
    void feed_external(int _w, int _h, int _c, int _n, void * _ptr);

    int copy_to_host(void * host_ptr, int host_offset, int this_offset, int nmemb);
    int copy_from_host(void * host_ptr, int host_offset, int this_offset, int nmemb);

    int copy(const tensor & other, int other_offset, int this_offset, int nmemb);
    int move(tensor & other);
    void * alloc(int nmemb);

    void free();

private:

    inline bool use_external_buffer() {return dm()==nullptr;}

    NAC_R_ATTR(void *, data)
    NAC_R_ATTR(int, w)
    NAC_R_ATTR(int, h)
    NAC_R_ATTR(int, c)
    NAC_R_ATTR(int, n)
    NAC_RW_ATTR(data_mm*, dm)
};

}

#endif