#include "context.h"
#include <mutex>


using namespace nac;


context context::INSTANCE;

context * context::get_context(){
    static  std::once_flag  _init_flag;
    try{
        std::call_once(_init_flag, init_once);
    }catch(...){

    }
    return &INSTANCE;
}

void context::init_once(){
    
}