#ifndef NAC_DATA_MM_H
#define NAC_DATA_MM_H

#include <string>

namespace nac{

class data_mm{
public:

// int are nmemb, not bytes.
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

    inline const std::string & name(){
        return name_;
    }
private:
    std::string name_;
};

}

#endif