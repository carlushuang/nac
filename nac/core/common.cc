#include "common.h"



namespace nac{


static struct {
    const char * str;
    int data_type;
} data_type_str_table [] = {
    {"fp32",    NAC_DATA_FP32},
    {"fp16",    NAC_DATA_FP16},
    {"dt1",     NAC_DATA_DEV_TYPE_1},
    {"dt2",     NAC_DATA_DEV_TYPE_2},
};
const char * data_type_to_str(int data_type){
    int total = sizeof(data_type_str_table)/sizeof(data_type_str_table[0]);
    for(int i=0;i<total;i++){
        if(data_type_str_table[i].data_type == data_type)
            return data_type_str_table[i].str;
    }
    return "n/a";
}
int str_to_data_type(const char * str){
    int total = sizeof(data_type_str_table)/sizeof(data_type_str_table[0]);
    std::string __str = str;
    for(int i=0;i<total;i++){
        if(data_type_str_table[i].str == __str)
            return data_type_str_table[i].data_type;
    }
    return NAC_DATA_MAX;
}


static struct {
    const char * str;
    activation_type act;
} act_type_str_table[] = {
    {"logistic",    activation_type::LOGISTIC},
    {"relu",        activation_type::RELU},
    {"relie",       activation_type::RELIE},
    {"linear",      activation_type::LINEAR},
    {"ramp",        activation_type::RAMP},
    {"tanh",        activation_type::TANH},
    {"plse",        activation_type::PLSE},
    {"leaky",       activation_type::LEAKY},
    {"elu",         activation_type::ELU},
    {"loggy",       activation_type::LOGGY},
    {"stair",       activation_type::STAIR},
    {"hardtan",     activation_type::HARDTAN},
    {"lhtan",       activation_type::LHTAN},
};

activation_type str_to_act_type(const char * str){
    std::string __str = str;
    int total = sizeof(act_type_str_table) / sizeof(act_type_str_table[0]);
    for(int i=0;i<total;i++){
        if(act_type_str_table[i].str == __str)
            return act_type_str_table[i].act;
    }
    return activation_type::UNKNOWN;
}
const char * act_type_to_str(activation_type act){
    int total = sizeof(act_type_str_table) / sizeof(act_type_str_table[0]);
    for(int i=0;i<total;i++){
        if(act_type_str_table[i].act == act)
            return act_type_str_table[i].str;
    }
    return "n/a";
}


}