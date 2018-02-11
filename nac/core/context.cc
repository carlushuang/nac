#include "context.h"
#include <mutex>

namespace nac{

//context context::INSTANCE;

std::mutex  context::dev_lock;
std::vector<unique_ptr<compute_device>>  context::dev_list;

context * context::get_context(){
    static  std::once_flag  _init_flag;
    try{
        std::call_once(_init_flag, init_once);
    }catch(...){

    }
    return &INSTANCE;
}

context::context(){
    static  std::once_flag  _init_flag;
    try{
        std::call_once(_init_flag, init_once);
    }catch(...){

    }
}
context::~context(){

}

void context::init_once(){

}

// only do once
void context::probe_compute_device(){
    dev_lock.lock();

    op_registry * default_c_op_regi = get_registry_entry("c");
    dev_list.emplace_back(new compute_device(default_c_op_regi));

    // TODO: dlopen to get dev from shlib

    dev_lock.unlock();
}

}
