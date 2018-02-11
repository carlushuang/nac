#ifndef NAC_OP_REGISTRY_H
#define NAC_OP_REGISTRY_H

#include <string>
#include <unordered_map>
#include <memory>
#include <operator_base>
#include <iostream>
#include <common.h>
#include <utility>

#define NAC_OP_REGISTRY_DECLARE(entry_name) \
    NAC_LOCAL op_registry * get_registry_##entry_name()

#define NAC_OP_REGISTRY_DEFINE(entry_name)  \
    NAC_LOCAL op_registry * get_registry_##entry_name(){ \
        static  op_registry  registry_##entry_name(#entry_name);   \
        return &registry_##entry_name; \
    }

#define NAC_OP_NAME(entry_name, data_type, op_name)  \
        #entry_name "-" #data_type "-"  #op_name 

#define NAC_OP_REGISTER(entry_name, data_type, op_name, op_class) \
    namespace { \
        static op_register NAC_CONCAT(_##entry_name, __LINE__) \
            (get_registry_##entry_name(), data_type, op_name, \
            new op_class( NAC_OP_NAME(entry_name, data_type, op_name) ) ) ; \
    }

#define NAC_GET_OP_REGISTRY(entry_name) get_registry_##entry_name()

#define NAC_DATA_FP32   0x0
#define NAC_DATA_FP16   0x1
#define NAC_DATA_MAX    4

namespace nac{

class op_registry{
public:
    typedef std::unordered_map<std::string, std::unique_ptr<operator_base>> op_map_type;
    typedef std::pair<int, op_map_type> data_entry_type;

    op_registry(const char * name){entry_name = name;}
    ~op_registry(){}

    inline void register_op(int data_type, std::string op_name, operator_base * op){
        int i;
        bool found_entry = false;
        if(data_type >= NAC_DATA_MAX){
            NAC_WARNING("try to insert data type ", data_type," exceed max ", NAC_DATA_MAX);
            return;
        }
        for(i=0;i<op_maps.size();i++){
            auto & en = op_maps[i];
            if(en.first == data_type){
                found_entry = true;
                break;
            }
        }
        if(!found_entry){
            op_maps.push_back(std::make_pair(data_type, op_map_type()));
            i = op_maps.size()-1;
        }

        op_map_type & op_map = op_maps[i].second;

        auto found = op_map.find(op_name);
        if(found != op_map.end()){
            std::cerr<<"have already registered op:"<<op_name<<
                " for registry:"<<entry_name<<" (" << data_type <<")"  <<std::endl;
            return;
        }
        std::unique_ptr<operator_base> new_op(op);
        op_map[op_name] = std::move(new_op);
    }
    inline op_map_type * get_ops(int data_type){
        int i;
        for(i=0;i<op_maps.size();i++){
            auto & en = op_maps[i];
            if(en.first == data_type){
                return & en.second;
            }
        }
        return nullptr;
    }
    inline operator_base * get_op(int data_type, const std::string op_name){
        auto op_map = get_ops(data_type);
        if(op_map == nullptr)
            return nullptr;
        auto found = op_map->find(op_name);
        if(found == op_map->end()){
            return nullptr;
        }
        return found.get();
    }

    inline const std::string & name() const{ return entry_name;}
    inline void release_unused(int data_type_keep){
        // release everything except the needed one, or all.
        auto it = op_maps.begin()
        if(data_type_keep<0){
            while(it!=op_maps.end()){
                it = op_maps.erase(it);
            }
        }
        else{
            while(it!=op_maps.end()){
                if(it->first == data_type_keep)
                    it++;   //keep
                else
                    it = op_maps.erase(it);
            }
        }
    }

private:
    std::string entry_name;
    op_registry(const op_registry&) = delete;
    op_registry& operator=(const op_registry&) = delete;

    //std::unordered_map<std::string, std::unique_ptr<operator_base>>  op_map;
    std::vector<data_entry_type> op_maps;
};

// helper class to register op
class op_register{
public:
    op_register(op_registry * opr, int data_type, std::string op_name, operator_base * op){
        opr->register_op(data_type, op_name, op);
    }
    ~op_register(){}
private:
    op_register(const op_register&) = delete;
    op_register& operator=(const op_register&) = delete;
    void * operator new(std::size_t) = delete;
    void * operator new[](std::size_t) = delete;
};

// functions
int insert_registry_entry(op_registry* opr);
op_registry * get_registry_entry(std::string entry_name);
const std::unordered_set<std::string> & supported_op_names();

int release_unused_entry(std::string entry_keep, int data_type_keep);
}

#endif