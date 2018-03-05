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
#include <atomic>

#define NAC_OP_REGISTRY_DECLARE(registry_name) \
    NAC_LOCAL op_registry * get_registry_##registry_name();

#define NAC_OP_REGISTRY_DEFINE(registry_name)  \
    NAC_LOCAL op_registry * get_registry_##registry_name(){ \
        static  op_registry  _registry_##registry_name(#registry_name);   \
        return &_registry_##registry_name; \
    };


#define NAC_OP_REGISTER(registry_name, data_type, op_name, op_class) \
    namespace { \
        static op_register NAC_CONCAT(_##registry_name, __LINE__) \
            (get_registry_##registry_name(), data_type, op_name, new op_class() ) ; \
    }

#define NAC_GET_OP_REGISTRY(registry_name) get_registry_##registry_name()

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


using  _nac_op_entry =  op_registry::op_entry_type;

class op_registry{
public:
    typedef std::unordered_map<std::string, std::unique_ptr<operator_base>> op_map_type;
    typedef std::pair<int, data_mm>     data_mm_type;

    class op_entry_type{
    public:
        op_registry       * registry;
        int                 data_type;  // key

        op_map_type         op_map;
        data_mm             op_dm;
        std::atomic_uint    ref_cnt;
    };

    static inline std::string gen_full_op_name(const char * _registry_name, const char * _data_type_str, const char * _op_name){
        std::string full_name;
        full_name += _registry_name;
        full_name += "-";
        full_name += _data_type_str;
        full_name += "-";
        full_name += _op_name;
        return full_name;
    }
    static inline std::string gen_full_data_mm_name(const char * _registry_name, const char * _data_type_str){
        std::string full_name;
        full_name += _registry_name;
        full_name += "-";
        full_name += _data_type_str;
        return full_name;
    }

    op_registry(const char * _name){name_=_name;}
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
        //try_locate_dm(data_type) = dm;
        auto & entry = try_locate_entry(data_type);
        auto & op_dm = entry.op_dm;

        // TODO: fix duplication
        op_dm = dm;

        return 0;
    }

    inline int register_op(int data_type, std::string op_name, operator_base * op){
        int i;
        bool found_entry = false;
        if(data_type >= NAC_DATA_MAX){
            NAC_WARNING("try to insert data type ", data_type," exceed max ", NAC_DATA_MAX);
            return -1;
        }

        auto & entry = try_locate_entry(data_type);
        auto & op_map = entry.op_map;

        auto found = op_map.find(op_name);
        if(found != op_map.end()){
            NAC_ERROR("have already registered op:", op->name());
            return -1;
        }
        std::unique_ptr<operator_base> new_op(op);
        op_map[op_name] = std::move(new_op);
        return 0;
    }

    inline op_entry_type * get_op_entry(const char * entry_name){
        return get_op_entry(str_to_data_type(entry_name));
    }

    inline op_entry_type * get_op_entry(int data_type){
        for(auto &oe : op_entries_){
            if(data_type == oe.data_type){
                oe.ref_cnt++;
                return &oe;
            }
        }
        return nullptr;
    }
    inline void put_op_entry(op_entry_type * _oe){
        for(auto &oe : op_entries_){
            if(_oe == &oe && _oe->registry == this){
                oe.ref_cnt--;
                break;
            }
        }
    }

    inline operator_base * get_op(op_entry_type * _oe, const char * _op_name){
        if(!_oe || !_op_name)
            return nullptr;
        NAC_ASSERT(this==_oe->registry, "it seems you try to query from a different registry");
        std::string _op_name_str = _op_name;
        auto found = _oe->op_map.find(_op_name_str);
        if(found == _oe->op_map.end())
            return nullptr;
        return found->second.get();
    }
    inline void put_op(operator_base * op){
        // TODO: ref cnt for op
        (void) op;
    }

    inline const std::vector<char *> &  op_entry_names() const {return op_entry_names_;}
    inline int op_entry_count() const {
        return op_entries_.size();
    }

    inline void assign_working_device(compute_device * _dev){
        dev_ = _dev;
        for(auto & oe : op_entries_){
            for(auto & kv : oe.op_map){
                kv.second->working_device() = _dev;
            }
        }
    }
    inline compute_device * & working_device() {
        return dev_;
    }

private:
    inline op_entry_type & try_locate_entry(int data_type){
        bool found_entry = false;
        int i;
        for(i=0;i<op_entries_.size();i++){
            if(op_entries_[i].data_type == data_type){
                found_entry = true;
                break;
            }
        }
        if(!found_entry){
            // store a ptr ** of op names for request use
            std::string _s = data_type_to_str(data_type);
            char * en = new char [_s.size()+1];
            std:copy(_s.begin(), _s.end(), en);
            op_entry_names_.push_back(en);

            // new empty entry
            op_entry_type new_entry;
            new_entry.data_type = data_type;
            new_entry.registry = this;
            op_entries_.push_back(std::move(new_entry));
            i=op_entries_.size()-1;
        }
        return op_entries_.at(i);
    }
    //std::unordered_map<std::string, std::unique_ptr<operator_base>>  op_map;
    std::vector<op_entry_type>      op_entries_;
    std::vector<char *>             op_entry_names_;
    compute_device *                dev_;

NAC_RW_ATTR(std::string, default_entry)
NAC_R_ATTR(std::string, name)
DISABLE_COPY_AND_ASSIGN(op_registry)
};

// helper class to register op
struct op_register{
    op_register(op_registry * opr, int data_type, std::string op_name, operator_base * op){
        op->full_record_name_ = op_registry::gen_full_op_name(opr->name().c_str(), data_type_to_str(data_type), op_name.c_str());
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

//------------------------------------------------------------------------

// functions
int insert_registry_entry(op_registry* opr);
op_registry * get_registry_entry(std::string entry_name);
const std::unordered_set<std::string> & supported_op_names();
bool check_op_supported(std::string op_name);

int release_unused_entry(std::string entry_keep, int data_type_keep);
}

#endif