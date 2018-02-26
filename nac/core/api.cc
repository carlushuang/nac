#include <nac.h>
#include "context.h"

/* export api */

using namespace nac;

static bool is_device_available(nac_device dev){
    std::vector<compute_device*> available_devs;
    probe_compute_devices(available_devs);
    bool in_list= false;
    for(auto & cdev : available_devs){
        if(cdev == device){
            in_list = true;
            break;
        }
    }
    return in_list;
}

NAC_EXPORT nac_context nac_create_context(nac_device *  devices, int num_device){
    if(!device || num_device==0 )
        return nullptr;
    for(int i=0;i<num_device;i++)
        if(!is_device_available(devices[i]))
            return nullptr;

    nac_context ctx = new context(devices, num_device);
    return ctx;
}

NAC_EXPORT nac_status nac_release_context(nac_context context){
    if(!context)
        return NAC_INVALID_ARG;
    delete context;
    return NAC_SUCCESS;
}

NAC_EXPORT nac_status nac_get_device_info(nac_device device, struct nac_device_info * info){
    if(!device || !info)
        return NAC_INVALID_ARG;
    if(!is_device_available(device))
        return NAC_INVALID_DEVICE;

    info->dev_name = device->name().c_str();

    info->num_op_entries = device->op_entries()->op_entry_count();
    info->op_entry_names = device->op_entries()->op_entry_names().data();
    std::string cur_en = device->current_entry_name();
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

NAC_EXPORT nac_status nac_select_op_entry(nac_context  context, const char * entry_name){
    if(!context || !entry_name)
        return NAC_INVALID_ARG;
    int ret = context->device()->select_op_entry(entry_name);
    if(ret != 0)
        return NAC_INVALID_OP_ENTRY_NAME;
    return NAC_SUCCESS;
}

NAC_EXPORT nac_operator nac_get_operator(nac_context context, const char * entry_name, const char * op_name){
    if(!context || !op_name || !entry_name)
        return NAC_INVALID_ARG;
    op_registry * opr = context->device()->op_entries();
    return opr->get_op(entry_name, op_name);
}

NAC_EXPORT nac_tensor nac_create_tensor(int w, int h, int c, int n){
    return new tensor(w,h,c,n);
}
NAC_EXPORT nac_status nac_release_tensor(nac_tensor tensor, void *release_data_func(void*)){
    if(!tensor)
        return NAC_INVALID_ARG;
    if(release_data_func)
        release_data_func(tensor->data());
    delete tensor;
    return NAC_SUCCESS;
}

NAC_EXPORT nac_status nac_set_tensor_data(nac_tensor tensor, void * data){
    if(!tensor)
        return NAC_INVALID_ARG;
    tensor->data() = data;
    return NAC_SUCCESS;
}
NAC_EXPORT nac_status nac_get_tensor_info(nac_tensor tensor, nac_tensor_info * info){
    if(!tensor || !info)
        return NAC_INVALID_ARG;
    info->w = tensor->w();
    info->h = tensor->h();
    info->c = tensor->c();
    info->n = tensor->n();
    infor->data = tensor->data();
    return NAC_SUCCESS;
}

NAC_EXPORT nac_layer nac_create_layer(nac_context context, nac_device device, const char * entry_name, const char * layer_name){
    if(!context || !device || !layer_name)
        return nullptr;

    if(!check_op_supported(layer_name)){
        NAC_ERROR("desired layer:", layer_name, " is not supported");
        return nullptr;
    }

    bool found = false;
    for(auto dev : context->devices()){
        if(dev == device){
            found = true;
            break;
        }
    }

    if(!found){
        NAC_ERROR("count not find the device");
        return nullptr;
    }

    op_registry * opr = device->op_entries();
    operator_base * op;

    op = opr->get_op(entry_name, layer_name  /*layer name same as op name*/ );
    if(!op){
        NAC_ERROR("count not find layer:", layer_name, " in device:", device->name());
        return nullptr;
    }

    layer * l = new layer(context);
    l->attach_op(op);

    return l;
}

NAC_EXPORT nac_status nac_release_layer(nac_layer layer){
    if(!layer)
        return NAC_INVALID_ARG;
    delete layer;
    return NAC_SUCCESS;
}
