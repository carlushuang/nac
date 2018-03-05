#ifndef _NAC_H
#define _NAC_H

#if (defined(__GNUC__) &&  __GNUC__ >= 4)
#define NAC_EXPORT __attribute__((visibility("default")))
#define NAC_LOCAL  __attribute__((visibility("hidden")))
#else
#define NAC_EXPORT
#define NAC_LOCAL
#endif

/* Error */
#define NAC_SUCCESS                          0
#define NAC_DEVICE_NOT_FOUND                -1
#define NAC_CONTEXT_CREATE_FAIL             -2
#define NAC_TENSOR_COPY_FAIL                -3
#define NAC_DUPLICATED_PARAM_NAME            -4

#define NAC_INVALID_ARG                      -100
#define NAC_INVALID_DEVICE                   -101
#define NAC_INVALID_OP_ENTRY_NAME            -102
#define NAC_INVALID_TENSOR                   -103
#define NAC_INVALID_LAYER_NAME               -104
#define NAC_INVALID_TENSOR_COPY_DIRECT       -105


#ifndef NAC_EXPORT_API_HIDDEN

namespace nac{

typedef class _nac_context      * nac_context;
typedef class _nac_operator     * nac_operator;
typedef class _nac_node         * nac_node;
typedef class _nac_device       * nac_device;
typedef class _nac_tensor       * nac_tensor;
typedef class _nac_graph        * nac_graph;
typedef class _nac_op_entry     * nac_op_entry;
typedef class _nac_hparam       * nac_hparam;

typedef int                     nac_status;

struct nac_device_info{
    const char      *   dev_name;
    const char      **  op_entry_names;
    const int           num_op_entries;
};
struct nac_tensor_info{
    int w,h,c,n;
    void * raw_data;
};


enum tensor_copy_direct {
    TENSOR_COPY_D2H = 0,
    TENSOR_COPY_H2D,
    TENSOR_COPY_D2D
};

NAC_EXPORT nac_status nac_get_devices(nac_device ** devices, int * num_devices);
NAC_EXPORT nac_status nac_get_device_info(nac_device dev, struct nac_device_info * info);
NAC_EXPORT nac_status nac_put_device(nac_device dev);

NAC_EXPORT nac_op_entry nac_get_op_entry(nac_device dev, const char * entry_name);
//NAC_EXPORT nac_status nac_get_op_entries(nac_device dev, nac_op_entry * entries, int * num_entries);
NAC_EXPORT nac_status nac_put_op_entry(nac_op_entry op_entry);

NAC_EXPORT nac_context nac_create_context(nac_device * devices, int num_device);
NAC_EXPORT nac_status nac_release_context(nac_context  ctx);

// select only one op_entry to be used in this context
// NAC_EXPORT nac_status nac_select_op_entry(nac_context ctx, const char * entry_name);

NAC_EXPORT nac_hparam nac_create_hparam(const char * op_name);
NAC_EXPORT nac_status nac_set_hparam(nac_hparam hparam, const char * param_name, const char * value);
NAC_EXPORT nac_status nac_release_hparam(nac_hparam hparam);

NAC_EXPORT nac_node nac_create_node(nac_context ctx, nac_op_entry op_entry, const char * op_name);
//NAC_EXPORT nac_status nac_set_node_input(nac_node nd, nac_tensor * inputs, int num_inputs);
NAC_EXPORT nac_status nac_feed_node_weight(nac_node nd, nac_tensor * weights, int num_weights);
NAC_EXPORT nac_status nac_set_node_hparam(nac_node nd, nac_hparam hparam);
NAC_EXPORT nac_status nac_release_node(nac_node nd);

NAC_EXPORT nac_graph nac_create_graph(nac_context ctx);
NAC_EXPORT nac_status nac_graph_attach_node(nac_graph gr, nac_node * nodes, int num);
NAC_EXPORT nac_status nac_graph_feed_input(nac_graph gr, nac_tensor * inputs, int num_inputs);
NAC_EXPORT nac_status nac_graph_init(nac_graph gr);
NAC_EXPORT nac_status nac_graph_finish(nac_graph gr);
NAC_EXPORT nac_status nac_graph_start_inference(nac_graph gr, int loop, int need_wait);
NAC_EXPORT nac_status nac_graph_wait(nac_graph gr);
NAC_EXPORT nac_status nac_graph_get_result(nac_graph gr, nac_tensor * out);
NAC_EXPORT nac_status nac_release_graph(nac_graph gr);

NAC_EXPORT nac_tensor nac_create_tensor(nac_op_entry op_entry, int w, int h, int c, int n);
NAC_EXPORT nac_status nac_release_tensor(nac_tensor t);
NAC_EXPORT nac_status nac_get_tensor_info(nac_tensor t, nac_tensor_info * info);
NAC_EXPORT nac_status nac_set_tensor_data_raw(nac_tensor t, void * data);
NAC_EXPORT nac_status nac_get_tensor_data_raw(nac_tensor t);
NAC_EXPORT nac_status nac_copy_tensor_data(void * src, int src_offset, void * dest, int dest_offset, 
                                            int nmemb, enum tensor_copy_direct direction);

}

#endif    // #ifndef NAC_EXPORT_API_HIDDEN

#endif
