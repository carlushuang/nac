#include <nac.h>
#include "ctx.h"

/* export api */

using namespace nac;

static bool is_device_available(nac_device dev){
    std::vector<compute_device*> available_devs;
    probe_compute_devices(available_devs);
    bool in_list= false;
    for(auto & cdev : available_devs){
        if(cdev == dev){
            in_list = true;
            break;
        }
    }
    return in_list;
}

NAC_EXPORT nac_context nac_create_context(nac_device *  devices, int num_device){
    if(!dev || num_device==0 )
        return nullptr;
    for(int i=0;i<num_device;i++)
        if(!is_device_available(devices[i]))
            return nullptr;

    nac_context ctx = new context(devices, num_device);
    return ctx;
}

NAC_EXPORT nac_status nac_release_context(nac_context ctx){
    if(!ctx)
        return NAC_INVALID_ARG;
    delete ctx;
    return NAC_SUCCESS;
}

NAC_EXPORT nac_status nac_get_device_info(nac_device dev, struct nac_device_info * info){
    if(!dev || !info)
        return NAC_INVALID_ARG;
    if(!is_device_available(dev))
        return NAC_INVALID_DEVICE;

    info->dev_name = dev->name().c_str();

    info->num_op_entries = dev->op_registry()->op_entry_count();
    info->op_entry_names = dev->op_registry()->op_entry_names().data();
    std::string cur_en = dev->current_entry_name();
    if(cur_en.empty())
        info->current_op_entry = (const char *)0;
    else
        info->current_op_entry = cur_en.c_str();
    return NAC_SUCCESS;
}

NAC_EXPORT nac_status nac_get_devices(nac_device ** devices, int * num_devices){
    nac_device * devs;
    int num_devs;
    if(!devices || !num_devices)
        return NAC_INVALID_ARG;
    probe_compute_devices(devs, num_devs);
    if(num_devs == 0)
        return NAC_DEVICE_NOT_FOUND;

    *devices = devs;
    *num_devices = num_devs;

    return NAC_SUCCESS;
}

NAC_EXPORT nac_status nac_select_op_entry(nac_context  ctx, const char * entry_name){
    if(!ctx || !entry_name)
        return NAC_INVALID_ARG;
    int ret = ctx->dev()->select_op_entry(entry_name);
    if(ret != 0)
        return NAC_INVALID_OP_ENTRY_NAME;
    return NAC_SUCCESS;
}

NAC_EXPORT nac_node nac_create_node(nac_context ctx, nac_op_entry op_entry,  const char * op_name){
    if(!ctx || ! op_entry || !op_name)
        return nullptr;
    if(!check_op_supported(op_name)){
        NAC_ERROR("request op:", op_name, " is not supported");
        return nullptr;
    }

    bool found = false;
    auto & dev =  op_entry->registry->working_device();
    for(auto & d : ctx->devices()){
        if(d == dev){
            found = true;
            break;
        }
    }

    if(!found){
        NAC_ERROR("count not find the dev");
        return nullptr;
    }

    op_registry * opr = dev->op_registry();
    operator_base * op;

    op = opr->get_op(op_entry, op_name  /*node name same as op name*/ );
    if(!op){
        NAC_ERROR("count not find node:", op_name, " in dev:", dev->name(), " with entry:", op_entry->name());
        return nullptr;
    }

    node * the_node = new node(ctx);
    the_node->attach_op(op);

    return the_node;
}
#if 0
NAC_EXPORT nac_status nac_release_node(nac_node nd){
    if(!nd)
        return NAC_INVALID_ARG;
    
}
#endif


NAC_EXPORT nac_tensor nac_create_tensor(int w, int h, int c, int n){
    return new tensor(w,h,c,n);
}
NAC_EXPORT nac_status nac_release_tensor(nac_tensor t){
    if(!t)
        return NAC_INVALID_ARG;

    delete t;
    return NAC_SUCCESS;
}

NAC_EXPORT nac_status nac_set_tensor_data(nac_tensor t, void * data){
    if(!t)
        return NAC_INVALID_ARG;
    t->data() = data;
    return NAC_SUCCESS;
}
NAC_EXPORT nac_status nac_get_tensor_info(nac_tensor t, nac_tensor_info * info){
    if(!t || !info)
        return NAC_INVALID_ARG;
    info->w = t->w();
    info->h = t->h();
    info->c = t->c();
    info->n = t->n();
    info->data = t->data();
    return NAC_SUCCESS;
}

NAC_EXPORT void * nac_get_tensor_data(nac_tensor t){
    if(!t)
        return nullptr;
    return t->data();
}

NAC_EXPORT nac_graph nac_create_graph(nac_context ctx){
    if(!ctx)
        return nullptr;
    nac_graph gr = new graph(ctx);
    return gr;
}
NAC_EXPORT nac_status nac_graph_attach_node(nac_graph gr, nac_node * nodes, int num){
    if(!gr || !nodes || num==0)
        return NAC_INVALID_ARG;
    gd->attach_nodes(nodes, num);
    return NAC_SUCCESS;
}
NAC_EXPORT nac_status nac_graph_feed_input(nac_graph gr, nac_tensor * inputs, int num_inputs){
    if(!gr || !nodes || num==0)
        return NAC_INVALID_ARG;
    gr->feed_inputs(inputs, num_inputs);
    return NAC_SUCCESS;
}
NAC_EXPORT nac_status nac_graph_init(nac_graph){

}
NAC_EXPORT nac_status nac_graph_start_inference(nac_graph gr, int loop){
    if(!gr || loop <= 0 )
        return NAC_INVALID_ARG;

    gr->inference_loop(loop);
    return NAC_SUCCESS;
}
NAC_EXPORT nac_status nac_graph_start_inference_async(nac_graph gr, int loop){

}
NAC_EXPORT nac_status nac_graph_get_result(nac_graph gr, nac_tensor ** outs, int * num_outs, int need_block){

}
NAC_EXPORT nac_status nac_release_graph(nac_graph gr){

}


NAC_EXPORT nac_layer nac_create_layer(nac_context ctx, nac_device dev, const char * entry_name, const char * layer_name){
    if(!ctx || !dev || !layer_name)
        return nullptr;

    if(!check_op_supported(layer_name)){
        NAC_ERROR("desired node:", layer_name, " is not supported");
        return nullptr;
    }

    bool found = false;
    for(auto d : ctx->devices()){
        if(d == dev){
            found = true;
            break;
        }
    }

    if(!found){
        NAC_ERROR("count not find the dev");
        return nullptr;
    }

    op_registry * opr = dev->op_registry();
    operator_base * op;

    op = opr->get_op(entry_name, layer_name  /*node name same as op name*/ );
    if(!op){
        NAC_ERROR("count not find node:", layer_name, " in dev:", dev->name());
        return nullptr;
    }

    node * l = new node(ctx);
    l->attach_op(op);

    return l;
}

NAC_EXPORT nac_status nac_release_layer(nac_layer node){
    if(!node)
        return NAC_INVALID_ARG;
    delete node;
    return NAC_SUCCESS;
}
