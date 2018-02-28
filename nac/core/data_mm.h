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

typedef int (*memcpy_t) (void * src, int src_offset, void * dest, int dest_offset, int nmemb);

    data_mm(const char* _name):
        name_(_name),
        allocator(nullptr), deleter(nullptr),
        memcpy_d2h(nullptr), memcpy_h2d(nullptr), memcpy_d2d(nullptr),
        unit(nullptr)
    {}
    data_mm():data_mm("N/A")
    {}

    void *  (*allocator)    (int nmemb);    // contiguous memory
    void    (*deleter)      (void* ptr);
    //int     (*copy)         (void * src, int src_offset, void * dest, int dest_offset, 
    //                            int nmemb, nac_memcpy_enum direction);
    memcpy_t memcpy_d2h;
    memcpy_t memcpy_h2d;
    memcpy_t memcpy_d2d;
    int     (*unit) ();

    

    const inline std::string & name(){
        return name_;
    }
private:
    std::string name_;
};

}

#endif