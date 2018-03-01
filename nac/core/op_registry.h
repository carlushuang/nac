#ifndef NAC_OP_REGISTRY_H
#define NAC_OP_REGISTRY_H


#include "operator_base.h"
#include "data_mm.h"
#include "common.h"

#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <utility>
#include <algorithm>

#define NAC_OP_REGISTRY_DECLARE(registry_name) \
    NAC_LOCAL op_registry * get_registry_##registry_name();

#define NAC_OP_REGISTRY_DEFINE(registry_name)  \
    NAC_LOCAL op_registry * get_registry_##registry_name(){ \
        static  op_registry  _registry_##registry_name(#registry_name);   \
        return &_registry_##registry_name; \
    };

#define NAC_OP_NAME(registry_name, data_type, op_name)  \
        #registry_name "-" #data_type "-"  #op_name 


#define NAC_OP_REGISTER(registry_name, data_type, op_name, op_class) \
    namespace { \
        static op_register NAC_CONCAT(_##registry_name, __LINE__) \
            (get_registry_##registry_name(), data_type, op_name, \
            new op_class( NAC_OP_NAME(registry_name, data_type, op_name) ) ) ; \
    }

#define NAC_GET_OP_REGISTRY(registry_name) get_registry_##registry_name()

#if 0
// refer to data_mm class
#define NAC_OP_REGISTER_DM_ALLOCATOR(registry_name, data_type, dm_allocator) \
    namespace { \
        static op_dm_register_allocator NAC_CONCAT(_##registry_name, __LINE__) \
            (NAC_GET_OP_REGISTRY(registry_name), data_type, dm_allocator) ; \
    }

#define NAC_OP_REGISTER_DM_DELETER(registry_name, data_type, dm_deleter) \
    namespace { \
        static op_dm_register_deleter NAC_CONCAT(_##registry_name, __LINE__) \
            (NAC_GET_OP_REGISTRY(registry_name), data_type, dm_deleter) ; \
    }

#define NAC_OP_REGISTER_DM_MEMCPY(registry_name, data_type, dm_memcpy_d2h, dm_memcpy_h2d, dm_memcpy_d2d) \
    namespace { \
        static op_dm_register_memcpy NAC_CONCAT(_##registry_name, __LINE__) \
            (NAC_GET_OP_REGISTRY(registry_name), data_type, dm_memcpy_d2h, dm_memcpy_h2d, dm_memcpy_d2d) ; \
    }

#define NAC_OP_REGISTER_DM_UNIT(registry_name, data_type, dm_unit) \
    namespace { \
        static op_dm_register_unit NAC_CONCAT(_##registry_name, __LINE__) \
            (NAC_GET_OP_REGISTRY(registry_name), data_type, dm_unit) ; \
    }

#endif

#define NAC_OP_REGISTER_DM(registery_name, data_type, dm_allocator, dm_deleter, \
                            dm_memcpy_d2h, dm_memcpy_h2d, dm_memcpy_d2d, \
                            dm_unit)  \
    namespace { \
        static op_dm_register NAC_CONCAT(_##registry_name, __LINE__) \
            (NAC_GET_OP_REGISTRY(registry_name), data_type, dm_allocator,   \
                dm_deleter, dm_memcpy_d2h, dm_memcpy_h2d, dm_memcpy_d2d, dm_unit) ; \
    }


namespace nac{
/*
 * device(class op_registry)
 *   |- entry/data_type 1 -> ops(conv/maxpool/...)
 *   |- entry/data_type 2 -> ops
 *   |- ...
 */

class op_registry{
public:
    typedef std::unordered_map<std::string, std::unique_ptr<operator_base>> op_map_type;
    //typedef std::pair<int, op_map_type> data_entry_type;
    typedef std::pair<int, data_mm>     data_mm_type;

    struct data_entry_type{
        int                 data_type;  // key

        op_map_type         op_map;
        data_mm             op_dm;
    };

    op_registry(const char * _name){name()=_name;}
    ~op_registry(){
        for(char * en : op_entry_names_)
            delete [] en;
    }
#if 0
    inline int register_op(const char * entry_name, std::string op_name, operator_base * op){
        int data_type = str_to_data_type(entry_name);
        if(data_type == NAC_DATA_MAX)
            return -1;
        return register_op(data_type, op_name, op);
    }
#endif
    inline int register_op_dm(int data_type, data_mm & dm){
        if(data_type >= NAC_DATA_MAX){
            NAC_WARNING("try to insert data type ", data_type," exceed max ", NAC_DATA_MAX);
            return -1;
        }
        try_locate_dm(data_type) = dm;
        return 0;
    }
#if 0
    inline int register_dm_allocator(int data_type, void * (*dm_allocator)(int )){
        if(data_type >= NAC_DATA_MAX){
            NAC_WARNING("try to insert data type ", data_type," exceed max ", NAC_DATA_MAX);
            return -1;
        }
        data_mm & dm = try_locate_dm(data_type);
        dm.allocator = dm_allocator;
        return 0;
    }
    inline int register_dm_deleter(int data_type, void (*dm_deleter) (void*)){
        if(data_type >= NAC_DATA_MAX){
            NAC_WARNING("try to insert data type ", data_type," exceed max ", NAC_DATA_MAX);
            return -1;
        }
        data_mm & dm = try_locate_dm(data_type);
        dm.deleter = dm_deleter;
        return 0;
    }
    inline int register_dm_deleter(int data_type, void (*dm_deleter) (void*)){
        if(data_type >= NAC_DATA_MAX){
            NAC_WARNING("try to insert data type ", data_type," exceed max ", NAC_DATA_MAX);
            return -1;
        }
        data_mm & dm = try_locate_dm(data_type);
        dm.deleter = dm_deleter;
        return 0;
    }

    inline int register_dm_memcpy(int data_type, data_mm::memcpy_t dm_memcpy_d2h, data_mm::memcpy_t dm_memcpy_h2d, data_mm::memcpy_t dm_memcpy_d2d){
        if(data_type >= NAC_DATA_MAX){
            NAC_WARNING("try to insert data type ", data_type," exceed max ", NAC_DATA_MAX);
            return -1;
        }
        data_mm & dm = try_locate_dm(data_type);
        dm.memcpy_d2h = dm_memcpy_d2h;
        dm.memcpy_h2d = dm_memcpy_h2d;
        dm.memcpy_d2d = dm_memcpy_d2d;
        return 0;
    }

    inline int register_dm_unit(int data_type, int (*dm_unit) ()){
        if(data_type >= NAC_DATA_MAX){
            NAC_WARNING("try to insert data type ", data_type," exceed max ", NAC_DATA_MAX);
            return -1;
        }
        data_mm & dm = try_locate_dm(data_type);
        dm.unit = dm_unit;
        return 0;
    }
#endif
    inline int register_op(int data_type, std::string op_name, operator_base * op){
        int i;
        bool found_entry = false;
        if(data_type >= NAC_DATA_MAX){
            NAC_WARNING("try to insert data type ", data_type," exceed max ", NAC_DATA_MAX);
            return -1;
        }
        for(i=0;i<op_maps_.size();i++){
            auto & en = op_maps_[i];
            if(en.first == data_type){
                found_entry = true;
                break;
            }
        }

        if(!found_entry){
            //op_entries.push_back(data_type);
            std::string _s = data_type_to_str(data_type);
            char * en = new char [_s.size()+1];
            std:copy(_s.begin(), _s.end(), en);
            op_entry_names_.push_back(en);
            op_maps_.push_back(std::make_pair(data_type, op_map_type()));
            i = op_maps_.size()-1;
            // make entry default when we add a new one
            default_entry() = data_type_to_str(data_type);
        }

        op_map_type & op_map = op_maps_[i].second;

        auto found = op_map.find(op_name);
        if(found != op_map.end()){
            std::cerr<<"have already registered op:"<<op_name<<
                " for registry:"<<name()<<" (" << data_type <<")"  <<std::endl;
            return -1;
        }
        std::unique_ptr<operator_base> new_op(op);
        op_map[op_name] = std::move(new_op);
        return 0;
    }
    inline op_map_type * get_ops(){
        int data_type = str_to_data_type(default_entry().c_str());
        return get_ops(data_type);
    }
    inline op_map_type * get_ops(const char * entry_name){
        int data_type = str_to_data_type(entry_name);
        return get_ops(data_type);
    }
    inline op_map_type * get_ops(int data_type){
        int i;
        for(i=0;i<op_maps_.size();i++){
            auto & en = op_maps_[i];
            if(en.first == data_type){
                return & en.second;
            }
        }
        return nullptr;
    }
    inline operator_base * get_op(const char * op_name){
        int data_type = str_to_data_type(default_entry().c_str());
        return get_op(data_type, op_name);
    }
    inline operator_base * get_op(const char * entry_name, const char * op_name){
        if(!entry_name)
            return get_op(op_name);
        int data_type = str_to_data_type(entry_name);
        return get_op(data_type, op_name);
    }
    inline operator_base * get_op(int data_type, const char *  op_name){
        std::string op_name_str = op_name;
        auto op_map = get_ops(data_type);
        if(op_map == nullptr)
            return nullptr;
        auto found = op_map->find(op_name_str);
        if(found == op_map->end()){
            return nullptr;
        }
        return found->second.get();
    }

#if 0
    inline void release_unused(int data_type_keep){
        // release everything except the needed one, or all.
        auto it = op_maps_.begin()
        if(data_type_keep<0){
            while(it!=op_maps_.end()){
                it = op_maps_.erase(it);
            }
        }
        else{
            while(it!=op_maps_.end()){
                if(it->first == data_type_keep)
                    it++;   //keep
                else
                    it = op_maps_.erase(it);
            }
        }
    }
#endif
    //inline std::vector<int> & get_op_entries() {return op_entries;}
    inline const std::vector<char *> &  op_entry_names() const {return op_entry_names_;}
    inline int op_entry_count() const {
        return op_maps_.size();
    }

    inline void assign_working_device(compute_device * dev){
        for(int i=0;i<op_maps_.size();i++){
            for(auto & kv : op_maps_[i].second){
                auto & op_ptr = kv.second;          // unique_ptr<>
                op_ptr->working_device() = dev;
            }
        }
    }

private:
    inline data_entry_type & try_locate_entry(int data_type){
        bool found_entry = false;
        int i;
        for(i=0;i<op_maps_.size();i++){
            if(op_maps_[i].data_type == data_type){
                found_entry = true;
                break;
            }
        }
        if(!found_entry){
            std::string _s = data_type_to_str(data_type);
            char * en = new char [_s.size()+1];
            std:copy(_s.begin(), _s.end(), en);
            op_entry_names_.push_back(en);

            data_entry_type new_entry;
            op_maps_.push_back(new_entry);
            i=op_maps_.size()-1;
        }
        return op_maps_.at(i);
    }
    //std::unordered_map<std::string, std::unique_ptr<operator_base>>  op_map;
    std::vector<data_entry_type>    op_maps_;
    std::vector<char *>             op_entry_names_;

NAC_RW_ATTR(std::string, default_entry)
NAC_R_ATTR(std::string, name)
DISABLE_COPY_AND_ASSIGN(op_registry)
};

// helper class to register op
struct op_register{
    op_register(op_registry * opr, int data_type, std::string op_name, operator_base * op){
        opr->register_op(data_type, op_name, op);
    }
};

//------------------------------------------------------------------------

struct op_dm_register{
    op_dm_register(op_registry * opr, int data_type, 
        data_mm::alloc_t alloc, data_mm::del_t del, 
        data_mm::cpy_t cpy_d2h, data_mm::cpy_t cpy_h2d, data_mm::cpy_t cpy_d2d,
        data_mm::unit_t unit ){

        data_mm dm;
        dm.allocator = alloc;
        dm.deleter = del;
        dm.memcpy_d2h = cpy_d2h;
        dm.memcpy_h2d = cpy_h2d;
        dm.memcpy_d2d = cpy_d2d;
        dm.unit = unit;
        opr->register_op_dm(dm);
    }
};

#if 0
struct op_dm_register_allocator{
    op_dm_register_allocator(op_registry * opr, int data_type,  void * (*dm_allocator)(int )){
        opr->register_dm_allocator(data_type, dm_allocator);
    }
};

struct op_dm_register_deleter{
    op_dm_register_deleter(op_registry * opr, int data_type,  void  (*dm_deleter)(void* )){
        opr->register_dm_deleter(data_type, dm_deleter);
    }
};

struct op_dm_register_memcpy{
    op_dm_register_memcpy(op_registry * opr, int data_type, 
        data_mm::memcpy_t dm_memcpy_d2h, data_mm::memcpy_t dm_memcpy_h2d, data_mm::memcpy_t dm_memcpy_d2d){
        opr->register_dm_memcpy(data_type, dm_memcpy_d2h, dm_memcpy_h2d, dm_memcpy_d2d);
    }
};

struct op_dm_register_unit{
    op_dm_register_unit(op_registry * opr, int data_type, int (*dm_unit) ()){
        opr->register_dm_unit(data_type, dm_unit);
    }
};
#endif
//------------------------------------------------------------------------

// functions
int insert_registry_entry(op_registry* opr);
op_registry * get_registry_entry(std::string entry_name);
const std::unordered_set<std::string> & supported_op_names();
bool check_op_supported(std::string op_name);

int release_unused_entry(std::string entry_keep, int data_type_keep);
}

#endif