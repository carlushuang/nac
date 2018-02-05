#include <nac_op_registry.h>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <string>

namespace nac{

static std::unordered_map<std::string, op_registry *> regi_entry_map;
static std::unordered_set<std::string> op_list ({
#define OP(x)  #x,
#include "supported_ops"
#undef  OP(x)
});


int insert_registry_entry(op_registry* opr){
    std::string entry_name = opr->name();
    auto found = regi_entry_map.find(entry_name);
    if(found == regi_entry_map.end()){
        regi_entry_map[entry_name] = opr;
        return 0;
    }
    NAC_WARNING("duplicate op entry:", entry_name);
    return -1;
}

op_registry * get_registry_entry(std::string entry_name){
    auto found = regi_entry_map.find(entry_name);
    if(found == regi_entry_map.end()){
        return nullptr;
    }
    return found->second;
}

const std::unordered_set<std::string> & supported_op_names(){
    return op_list;
}

class __default_op_entry{
public:
    __default_op_entry(){
        /*
         * register the default c version operator
         */
        insert_registry_entry(NAC_GET_OP_REGISTRY(c));
    }
};
static __default_op_entry  default_op_entry;

}