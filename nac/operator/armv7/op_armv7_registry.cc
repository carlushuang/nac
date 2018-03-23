#include "op_armv7_registry.h"
#include <cstring>

namespace nac{
NAC_OP_REGISTRY_DEFINE(armv7)

static inline int fp32_memcpy_armv7(void * src, int src_offset, void * dest, int dest_offset, int nmemb){
#if __ARM_NEON
    void * s = (void*)((float*)src + src_offset);
    void * d = (void*)((float*)dest + dest_offset);
    int bytes = nmemb * sizeof(float);
    int remain = bytes & (64-1);    // mod 64
    bytes -= remain;
    //http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka13544.html
    asm volatile(
        "0:                 \n"
        "pld [%1, #0xc0]   \n"    // 3 cacheline ahead (64Byte cacheline size)
        "vldm %1!, {d0-d7} \n"
        "vstm %0!, {d0-d7} \n"
        "subs %2, %2, #0x40\n"
        "bgt 0b\n"
        :"=r"(d),           // %0
         "=r"(s),           // %1
         "=r"(bytes)        // %2
        :"0"(d),
         "1"(s),
         "2"(bytes)
        :"cc", "memory", "d0","d1","d2","d3","d4","d5","d6","d7"
    );
    if(remain){
        std::memcpy(d, s, remain);
    }
    return 0;
#else
    void * x = (void*)((float*)src + src_offset);
    void * y = (void*)((float*)dest + dest_offset);
    std::memcpy(y, x, nmemb * sizeof(float));
    return 0;
#endif
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
        return fp32_memcpy_armv7(src, src_offset, dest, dest_offset, nmemb );
    },
    // memcpy_h2d
    [](void * src, int src_offset, void * dest, int dest_offset, int nmemb) -> int{
        return fp32_memcpy_armv7(src, src_offset, dest, dest_offset, nmemb );
    },
    // memcpy_d2d
    [](void * src, int src_offset, void * dest, int dest_offset, int nmemb) -> int{
        return fp32_memcpy_armv7(src, src_offset, dest, dest_offset, nmemb );
    },
    // unit
    []()->int{
        return static_cast<int>(sizeof(float));
    })
}