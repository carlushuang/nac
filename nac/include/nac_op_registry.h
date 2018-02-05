#ifndef NAC_OP_REGISTRY_H
#define NAC_OP_REGISTRY_H

#include <string>
#include <unordered_map>
#include <memory>
#include <operator_base>
#include <iostream>
#include <nac_internal.h>


#define NAC_OP_REGISTRY_DECLARE(entry_name) \
    NAC_LOCAL op_registry * get_registry_##entry_name()

#define NAC_OP_REGISTRY_DEFINE(entry_name)  \
    NAC_LOCAL op_registry * get_registry_##entry_name(){ \
        static  op_registry  registry_##entry_name(entry_name);   \
        return &registry_##entry_name; \
    }

#define NAC_OP_REGISTER(entry_name, op_name, op_class) \
    namespace { \
        static op_register NAC_CONCAT(_##entry_name, __LINE__) \
            (get_registry_##entry_name(), op_name, new op_class) ; \
    }

#define NAC_GET_OP_REGISTRY(entry_name) get_registry_##entry_name()

namespace nac{

class op_registry{
public:
    op_registry(const char * name){entry_name = name;}
    ~op_registry(){}

    inline void register_op(std::string op_name, operator_base * op){
        auto found = op_map.find(op_name);
        if(found == op_map.end()){
            std::cerr<<"have already registered op:"<<op_name<<
                " for registry:"<<entry_name<<std::endl;
            return;
        }
        std::unique_ptr<operator_base> new_op(op);
        op_map[op_name] = std::move(new_op);
    }
    inline operator_base * get_op(const std::string op_name){
        auto found = op_map.find(op_name);
        if(found == op_map.end()){
            return nullptr;
        }
        return found.get();
    }

    inline const std::string & name() const{ return entry_name;}

private:
    std::string entry_name;
    op_registry(const op_registry&) = delete;
    op_registry& operator=(const op_registry&) = delete;

    std::unordered_map<std::string, std::unique_ptr<operator_base>>  op_map;
};

// helper class to register op
class op_register{
public:
    op_register(op_registry * oe, std::string op_name, operator_base * op){
        oe->register_op(op_name, op);
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

}

#endif