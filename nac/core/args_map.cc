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
