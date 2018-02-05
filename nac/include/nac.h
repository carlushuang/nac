#ifndef _NAC_H
#define _NAC_H


#if (defined(__GNUC__) &&  __GNUC__ >= 4)
#define NAC_EXPORT __attribute__((visibility("default")))
#define NAC_LOCAL  __attribute__((visibility("hidden")))
#else
#define NAC_EXPORT
#define NAC_LOCAL
#endif

namespace nac{

typedef class _nac_context * nac_context;

NAC_EXPORT nac_context nac_get_context();



}

#endif