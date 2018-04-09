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
#define NAC_DEVICE_NOT_FOUND                 -1
#define NAC_CONTEXT_CREATE_FAIL              -2
#define NAC_TENSOR_COPY_FAIL                 -3
#define NAC_DUPLICATED_PARAM_NAME            -4

#define NAC_INVALID_ARG                      -100
#define NAC_INVALID_DEVICE                   -101
#define NAC_INVALID_OP_ENTRY_NAME            -102
#define NAC_INVALID_TENSOR                   -103
#define NAC_INVALID_LAYER_NAME               -104
#define NAC_INVALID_TENSOR_COPY_DIRECT       -105


#ifdef __cplusplus
extern "C" {
#endif

#ifndef NAC_EXPORT_API_HIDDEN


typedef void *        nac_context;
typedef void *        nac_operator;
typedef void *        nac_node;
typedef void *        nac_device;
typedef void *        nac_tensor;
typedef void *        nac_graph;
typedef void *        nac_op_entry;
typedef void *        nac_hparam;
typedef void *        nac_brew;

typedef int           nac_status;

struct nac_device_info{
    char      *   dev_name;
    char      **  op_entry_names;
    int           num_op_entries;
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

NAC_EXPORT nac_status   nac_device_get(nac_device ** devices, int * num_devices);
NAC_EXPORT nac_status   nac_device_get_info(nac_device dev, struct nac_device_info * info);
NAC_EXPORT nac_status   nac_device_put(nac_device dev);

NAC_EXPORT nac_op_entry nac_op_entry_get(nac_device dev, const char * entry_name);
//NAC_EXPORT nac_status nac_get_op_entries(nac_device dev, nac_op_entry * entries, int * num_entries);
NAC_EXPORT nac_status   nac_op_entry_put(nac_op_entry op_entry);

NAC_EXPORT nac_context  nac_context_create(nac_device * devices, int num_device);
NAC_EXPORT nac_status   nac_context_release(nac_context  ctx);


NAC_EXPORT nac_hparam   nac_hparam_create(const char * op_name);
NAC_EXPORT nac_status   nac_hparam_set(nac_hparam hparam, const char * param_name, const char * value);
NAC_EXPORT nac_status   nac_hparam_release(nac_hparam hparam);

// brew model
/* args should be: "<key>=<value>:<key>=<value>...""  */
NAC_EXPORT nac_brew     nac_brew_create(nac_context ctx, const char * args);
NAC_EXPORT nac_status   nac_brew_release(nac_brew br);

// TODO: node dtor managed by graph, no need to call release
NAC_EXPORT nac_node     nac_node_create(nac_context ctx, nac_op_entry op_entry, const char * op_name);
NAC_EXPORT nac_status   nac_node_feed_weights(nac_node nd, nac_tensor * weights, int num_weights);
NAC_EXPORT nac_status   nac_node_set_hparam(nac_node nd, nac_hparam hparam);
NAC_EXPORT nac_status   nac_node_release(nac_node nd);

NAC_EXPORT nac_graph    nac_graph_create(nac_context ctx);
NAC_EXPORT nac_status   nac_graph_attach_node(nac_graph gr, nac_node * nodes, int num);
NAC_EXPORT nac_status   nac_graph_feed_inputs(nac_graph gr, nac_tensor * inputs, int num_inputs);
NAC_EXPORT nac_status   nac_graph_prepare(nac_graph gr);
NAC_EXPORT nac_status   nac_graph_finish(nac_graph gr);
NAC_EXPORT nac_status   nac_graph_start_inference(nac_graph gr, int loop, int need_wait);
NAC_EXPORT nac_status   nac_graph_wait(nac_graph gr);
NAC_EXPORT nac_status   nac_graph_get_result(nac_graph gr, nac_tensor * out);
NAC_EXPORT nac_status   nac_graph_release(nac_graph gr);

NAC_EXPORT nac_tensor   nac_tensor_create(nac_op_entry op_entry, int w, int h, int c, int n);
NAC_EXPORT nac_status   nac_tensor_release(nac_tensor t);
NAC_EXPORT nac_status   nac_tensor_get_info(nac_tensor t, nac_tensor_info * info);
NAC_EXPORT nac_status   nac_tensor_set_data_raw(nac_tensor t, void * data);
NAC_EXPORT void *       nac_tensor_get_data_raw(nac_tensor t);
NAC_EXPORT nac_status   nac_tensor_copy_data(void * src, int src_offset, void * dest, int dest_offset, 
                                            int nmemb, enum tensor_copy_direct direction);


#endif    // #ifndef NAC_EXPORT_API_HIDDEN

#ifdef __cplusplus
}  //extern "C" {
#endif

#endif
