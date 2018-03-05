#include "op_armv7_registry.h"
#include <cstring>

namespace nac{
NAC_OP_REGISTRY_DEFINE(armv7)

static inline int fp32_memcpy(void * src, int src_offset, void * dest, int dest_offset, int nmemb){
    void * x = (void*)((float*)src + src_offset);
    void * y = (void*)((float*)dest + dest_offset);
    std::memcpy(y, x, nmemb * sizeof(float));
    return 0;
}

NAC_OP_REGISTER_DM(armv7, NAC_DATA_FP32,
    // allocator
    [](int nmemb) -> void * {
        return (void *)new float [nmemb];
    },
    // deleter
    [](void * ptr) -> void {
        delete [] (float*)ptr;
    },
    // memcpy_d2h
    [](void * src, int src_offset, void * dest, int dest_offset, int nmemb) -> int{
        return fp32_memcpy(src, src_offset, dest, dest_offset, nmemb );
    },
    // memcpy_h2d
    [](void * src, int src_offset, void * dest, int dest_offset, int nmemb) -> int{
        return fp32_memcpy(src, src_offset, dest, dest_offset, nmemb );
    },
    // memcpy_d2d
    [](void * src, int src_offset, void * dest, int dest_offset, int nmemb) -> int{
        return fp32_memcpy(src, src_offset, dest, dest_offset, nmemb );
    },
    // unit
    []()->int{
        return static_cast<int>(sizeof(float));
    })
}