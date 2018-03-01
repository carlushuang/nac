#ifndef NAC_DATA_MM_H
#define NAC_DATA_MM_H

#include <string>


namespace nac{

#if 0
enum class nac_memcpy_enum{
    h2h = 0,    // host to host
    h2d,        // host to device
    d2h,        // device to host
    d2d,        // device to device
};
#endif

class data_mm{
public:

typedef void *  (*alloc_t)  (int);
typedef void    (*del_t)    (void*);
// src_offset/dest_offset are not byte, they are number of element
typedef int     (*cpy_t)    (void * src, int src_offset, void * dest, int dest_offset, int nmemb);
typedef int     (*unit_t)   ();

    data_mm(const char* _name):
        name_(_name),
        allocator(nullptr), deleter(nullptr),
        memcpy_d2h(nullptr), memcpy_h2d(nullptr), memcpy_d2d(nullptr),
        unit(nullptr)
    {}
    data_mm():data_mm("N/A")
    {}

    alloc_t     allocator;
    del_t       deleter;
    cpy_t       memcpy_d2h;
    cpy_t       memcpy_h2d;
    cpy_t       memcpy_d2d;
    unit_t      unit;

    const inline std::string & name(){
        return name_;
    }
private:
    const std::string name_;
};

}

#endif