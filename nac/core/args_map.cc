#include "args_map.h"
#include "common.h"

namespace nac{

#define AM_FIND_BODY(type, cvt, def)                \
    type ret;                                       \
    if(arg_map_.find(arg_name)!=arg_map_.end()){                  \
        try{                                            \
            ret = cvt(arg_map_[arg_name]);             \
        }catch(...){                                    \
            NAC_ASSERT(0, __func__, " convertion from ", arg_map_[arg_name], " fail");   \
            ret = def;                                      \
        }                                                   \
    }else{                                                  \
        ret = def;                                          \
    }                                                       \
    return ret

std::string args_map::find(std::string arg_name){
    if(arg_map_.find(arg_name)!=arg_map_.end()){
        return arg_map_[arg_name];
    }else{
        NAC_ASSERT(0, "can not find value for:", arg_name);
    }
    // never reach hear
    return "";
}

std::string args_map::find(std::string arg_name, std::string def_value){
    AM_FIND_BODY(std::string, std::string, def_value);
}

bool args_map::parse(const char * args){
    std::string args_str(args);
    const char * sentence_token=":;";
    const char * pair_token="=";
    int pair_cnt = 0;
    std::size_t sentense_end;
    std::size_t sentense_start = 0;
    std::size_t pair_pos;
    pair_pos = args_str.find_first_of(pair_token, sentense_start);
    while(pair_pos!=std::string::npos){
        sentense_end = args_str.find_first_of(sentence_token, sentense_start);  // may be npos
        std::string key = args_str.substr(sentense_start, pair_pos-sentense_start);
        std::string value;
        if(sentense_end == std::string::npos)
            value = args_str.substr(pair_pos+1, sentense_end);  // till the end
        else
            value = args_str.substr(pair_pos+1, sentense_end-pair_pos-1);
        std::cout<<__func__<<": \""<<key<<"\"=\""<<value<<"\""<<std::endl;
        this->insert(key, value);
        pair_cnt++;
        if(sentense_end == std::string::npos)
            break;
        else{
            sentense_start = sentense_end+1;
            pair_pos = args_str.find_first_of(pair_token, sentense_start);
        }
    }
    return pair_cnt>0;
}

void args_map::clear(){
    arg_map_.clear();
}

int     args_map::find_int(std::string arg_name, int def_value){
    AM_FIND_BODY(int, std::stoi, def_value);
}
float   args_map::find_float(std::string arg_name, float def_value){
    AM_FIND_BODY(float, std::stof, def_value);
}
double  args_map::find_double(std::string arg_name, double def_value){
    AM_FIND_BODY(double, std::stod, def_value);
}

}
