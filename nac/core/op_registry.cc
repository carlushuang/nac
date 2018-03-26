#include "op_registry.h"
#include "operator_base.h"
#include "data_mm.h"
#include "compute_device.h"
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <string>

namespace nac{

static std::unordered_map<std::string, op_registry *> regi_entry_map;
static std::unordered_set<std::string> op_list ({
#define OP(x)  #x,
#include <supported_ops>
#undef  OP
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

int release_unused_entry(std::string entry_keep, int data_type_keep){
#if 0
    for(auto & it : regi_entry_map){

        if(it.first == entry_keep)
            it.second->release_unused(entry_keep);  // keep one, release else
        else
            it.second->release_unused(-1);  // release all
    }
#endif
    op_list.clear();
    return 0;
}
bool check_op_supported(std::string op_name){
    auto found = op_list.find(op_name);
    if(found == op_list.end())
        return false;
    return true;
}

NAC_OP_REGISTRY_DECLARE(c);
class __default_op_entry{
public:
    __default_op_entry(){
        /*
         * register the default c version operator
         */
        insert_registry_entry(NAC_GET_OP_REGISTRY(c));
    }
};

namespace {
static __default_op_entry  default_op_entry;
}



op_registry::op_entry_type::op_entry_type(op_registry * _regi, int _dt) : 
        registry(_regi), data_type(_dt), ref_cnt(0)
{}



op_registry::op_registry(const char * _name){name_=_name;}
op_registry::~op_registry(){
    for(char * en : op_entry_names_)
        delete [] en;
}
#if 0
int register_op(const char * entry_name, std::string op_name, operator_base * op){
    int data_type = str_to_data_type(entry_name);
    if(data_type == NAC_DATA_MAX)
        return -1;
    return register_op(data_type, op_name, op);
}
#endif
int op_registry::register_op_dm(int data_type, data_mm & dm){
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

int op_registry::register_op(int data_type, std::string op_name, operator_base * op){
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

op_registry::op_entry_type * op_registry::get_op_entry(const char * entry_name){
    return get_op_entry(str_to_data_type(entry_name));
}

op_registry::op_entry_type * op_registry::get_op_entry(int data_type){
    for(auto &oe : op_entries_){
        if(data_type == oe.data_type){
            oe.ref_cnt++;
            return &oe;
        }
    }
    return nullptr;
}
void op_registry::put_op_entry(op_registry::op_entry_type * _oe){
    for(auto &oe : op_entries_){
        if(_oe == &oe && _oe->registry == this){
            oe.ref_cnt--;
            break;
        }
    }
}

operator_base * op_registry::get_op(op_registry::op_entry_type * _oe, const char * _op_name){
    if(!_oe || !_op_name)
        return nullptr;
    NAC_ASSERT(this==_oe->registry, "it seems you try to query from a different registry");
    std::string _op_name_str = _op_name;
    auto found = _oe->op_map.find(_op_name_str);
    if(found == _oe->op_map.end())
        return nullptr;
    return found->second.get();
}
void op_registry::put_op(operator_base * op){
    // TODO: ref cnt for op
    (void) op;
}

const std::vector<char *> &  op_registry::op_entry_names() const 
{
    return op_entry_names_;
}
int op_registry::op_entry_count() const {
    return op_entries_.size();
}

void op_registry::assign_working_device(compute_device * _dev){
    dev_ = _dev;
    for(auto & oe : op_entries_){
        for(auto & kv : oe.op_map){
            kv.second->working_device() = _dev;
        }
    }
}
compute_device * & op_registry::working_device() {
    return dev_;
}

op_registry::op_entry_type & op_registry::try_locate_entry(int data_type){
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
        int length = _s.size();
        char * en = new char [length+1];
        //std::copy(_s.begin(), _s.end(), en);
        _s.copy(en,length);
        en[length] = '\0';
        op_entry_names_.push_back(en);

        // new empty entry
        //op_registry::op_entry_type new_entry;
        //new_entry.data_type = data_type;
        //new_entry.registry = this;
        //op_entries_.push_back(std::move(new_entry));
        op_entries_.emplace_back(this, data_type);
        i=op_entries_.size()-1;
    }
    return op_entries_.at(i);
}

op_register::op_register(op_registry * opr, int data_type, std::string op_name, operator_base * op){
    op->full_record_name_ = op_registry::gen_full_op_name(opr->name().c_str(), data_type_to_str(data_type), op_name.c_str());
    opr->register_op(data_type, op_name, op);
}

op_dm_register::op_dm_register(op_registry * opr, int data_type, 
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
    opr->register_op_dm(data_type, dm);
}


}
