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

typedef class _nac_context      * nac_context;
typedef class _nac_operator     * nac_operator;
typedef class _nac_layer        * nac_layer;
typedef class _nac_device       * nac_device;
typedef class _nac_tensor       * nac_tensor;

typedef int                     nac_status;


/* Error */
#define NAC_SUCCESS                          0
#define NAC_DEVICE_NOT_FOUND                -1
#define NAC_CONTEXT_CREATE_FAIL             -2

#define NAC_INVALID_ARG                      -100
#define NAC_INVALID_DEVICE                   -101
#define NAC_INVALID_OP_ENTRY_NAME            -102
#define NAC_INVALID_TENSOR                   -103
#define NAC_INVALID_LAYER_NAME               -104

struct nac_device_info{
    const char      *   dev_name;
    const char      **  op_entry_names;
    const int           num_op_entries;
    const char      *   current_op_entry;
};
struct nac_tensor_info{
    int w,h,c,n;
    void * data;
};

NAC_EXPORT nac_status nac_get_devices(nac_device ** devices, int * num_devices);
NAC_EXPORT nac_status nac_get_device_info(nac_device device, struct nac_device_info * info);

NAC_EXPORT nac_context nac_create_context(nac_device *  devices, int num_device);
NAC_EXPORT nac_status nac_release_context(nac_context  context);

// select only one op_entry to be used in this context
NAC_EXPORT nac_status nac_select_op_entry(nac_context context, const char * entry_name);

NAC_EXPORT nac_operator nac_get_operator(nac_context context, const char * entry_name,  const char * op_name);
NAC_EXPORT nac_status nac_operator_forward(nac_operator op, 
    nac_tensor * inputs, int num_input, nac_tensor * outputs, int num_output);

NAC_EXPORT nac_layer nac_create_layer(nac_context context, nac_device  device, const char * entry_name, const char * layer_name);
NAC_EXPORT nac_status nac_release_layer(nac_layer layer);

NAC_EXPORT nac_tensor nac_create_tensor(int w, int h, int c, int n);
NAC_EXPORT nac_status nac_release_tensor(nac_tensor tensor, void *release_data_func(void*));
NAC_EXPORT nac_status nac_set_tensor_data(nac_tensor tensor, void * data);
NAC_EXPORT nac_status nac_get_tensor_info(nac_tensor tensor, nac_tensor_info * info);
}

#endif
