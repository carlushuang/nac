#ifndef NAC_TENSOR_MM_H
#define NAC_TENSOR_MM_H

#include <string>

#define NAC_TENSOR_MM_DECLARE(entry_name) \
    NAC_LOCAL op_registry * get_tensor_mm_##entry_name()

#define NAC_TENSOR_MM_DEFINE(entry_name)  \
    NAC_LOCAL op_registry * get_registry_##entry_name(){ \
        static  op_registry  registry_##entry_name(#entry_name);   \
        return &registry_##entry_name; \
    }


namespace nac{

enum class nac_memcpy_enum{
    h2h = 0,    // host to host
    h2d,        // host to device
    d2h,        // device to host
    d2d,        // device to device
};

struct tensor_mm{

    tensor_mm() : allocator(nullptr), deleter(nullptr), copy(nullptr), unit(nullptr)
    {}

    void *  (*allocator)    (int nmemb);    // contiguous memory
    void    (*deleter)      (void* ptr);
    int     (*copy)         (void * src, int src_offset, void * dest, int dest_offset, 
                                int nmemb, nac_memcpy_enum direction);
    int     (*unit)         ();

    std::string name;
};

}

#endif