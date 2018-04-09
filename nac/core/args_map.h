#ifndef __ARGS_MAP_H
#define __ARGS_MAP_H

#include <unordered_map>
#include <string>
#include "common.h"

namespace nac {

class args_map{
public:
    args_map(){}
    virtual ~args_map(){}
    // safe for data type like int, long, float, double...
    template<typename value_type>
    int insert(std::string arg_name, value_type value){
        std::string val_str = std::to_string(value);
        if(arg_map_.find(arg_name) != arg_map_.end()){
            NAC_WARNING("try to insert duplicated arg:", arg_name, " with value:", 
                val_str, ", origin value is:", arg_map_[arg_name]);
        }
        arg_map_[arg_name] = val_str;
        return 0;
    }

    // parse "<key>=<value>:<key>=<value>..."" pair
    bool parse(const char * args);

    virtual void clear() final;
    virtual std::string find(std::string arg_name) final;   // if not find, terminate
    virtual std::string find(std::string arg_name, std::string def_value) final;
    virtual int         find_int(std::string arg_name, int def_value) final;
    virtual float       find_float(std::string arg_name, float def_value) final;
    virtual double      find_double(std::string arg_name, double def_value) final;

typedef std::unordered_map<std::string, std::string>::iterator iterator;
typedef std::unordered_map<std::string, std::string>::const_iterator const_iterator;

    iterator begin() {return arg_map_.begin();}
    iterator end() {return arg_map_.end();}
    const_iterator begin() const {return arg_map_.begin();}
    const_iterator end() const {return arg_map_.end();}

private:
    std::unordered_map<std::string, std::string>  arg_map_;
};

template<>
inline int args_map::insert (std::string arg_name, std::string value){
    if(arg_map_.find(arg_name) != arg_map_.end()){
        NAC_WARNING("try to insert duplicated arg:", arg_name, " with value:",
            value, ", origin value is:", arg_map_[arg_name]);
    }
    arg_map_[arg_name] = value;
    return 0;
}
template<>
inline int args_map::insert (std::string arg_name, const char* value){
    if(arg_map_.find(arg_name) != arg_map_.end()){
        NAC_WARNING("try to insert duplicated arg:", arg_name, " with value:",
            value, ", origin value is:", arg_map_[arg_name]);
    }
    arg_map_[arg_name] = value;
    return 0;
}

}

#endif
