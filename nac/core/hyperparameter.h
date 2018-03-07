#ifndef NAC_HYPERPARAMETER_H
#define NAC_HYPERPARAMETER_H

#include "common.h"
#include "node.h"
#include "tensor.h"
#include <unordered_map>
#include <string>

namespace nac{

class hparam_map;
class hyperparameter{
public:
    hyperparameter(){}
    ~hyperparameter(){}
    virtual void calculate_outsize(int * out_w, int * out_h, int * out_c, int * out_n) final{
        NAC_ASSERT(op_node() && op_node()->input(), "op_node must be attached, and must have at least one input");
        NAC_ASSERT(out_w && out_h && out_c && out_n, "input param must not be nullptr");
        outsize(out_w, out_h ,out_c ,out_n);
    }
    virtual void map(hparam_map * pmap) = 0;
private:
    virtual void outsize(int * out_w, int * out_h, int * out_c, int * out_n) = 0;

NAC_RW_ATTR(int, batch)
NAC_RW_ATTR(node*, op_node)
};

class hparam_map{
public:
    hparam_map(const char * for_op_name) : name_(for_op_name) {}
    inline int insert_param(const char * param_name, const char * value){
        if(param_map.count(param_name) == 0){
            param_map.insert(std::make_pair<std::string, std::string>(param_name, value));
            return 0;
        }
        return -1;
    }
    const char * find_param(const char * param_name) const{
        if(param_map.count(param_name) == 0){
            return nullptr;
        }
        return param_map.at(param_name).c_str();
    }
private:
    std::unordered_map<std::string, std::string>   param_map;
    NAC_R_ATTR(std::string, name)
};


inline int hparam_to_int(const char * value){
    NAC_ASSERT(value);
    try{
        return std::stoi(value);
    }catch(...){
        NAC_ASSERT(0, __func__, " convertion from ", value, " fail");
    }
    return -1;
}
inline activation_type hparam_to_act(const char * value){
    NAC_ASSERT(value);

    return str_to_act_type(value);
}
inline float hparam_to_float(const char * value){
    NAC_ASSERT(value);
    try{
        return std::stof(value);
    }catch(...){
        NAC_ASSERT(0, __func__, " convertion from ", value, " fail");
    }
    return .0f;
}


#define  _NAC_INCLUDE_HYPERPARAMETER_DETAIL
#include "hyperparameter/conv_hparam.h"
#include "hyperparameter/activation_hparam.h"
#include "hyperparameter/softmax_hparam.h"
#include "hyperparameter/maxpool_hparam.h"
#undef   _NAC_INCLUDE_HYPERPARAMETER_DETAIL

inline hyperparameter * hyperparameter_factory(const char * bare_op_name){
    NAC_ASSERT(bare_op_name);
    std::string s(bare_op_name);
    if(s == "conv" )
        return new conv_hparam;
    if(s == "activation")
        return new activation_hparam;
    if(s == "softmax")
        return new softmax_hparam;
    if(s == "maxpool")
        return new maxpool_hparam;
    
    return nullptr;
}


}

#endif
