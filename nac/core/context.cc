#include "context.h"
#include "compute_device.h"
#include <mutex>

namespace nac{


context::context(compute_device ** _devs, int _num_devs){
#if 0
    static  std::once_flag  _init_flag;
    try{
        std::call_once(_init_flag, init_once);
    }catch(...){

    }
#endif
    if(!_devs || _num_devs==0){
        NAC_WARNING("no device specified while creating context!");
        return;
    }
    for(int i=0;i<_num_devs;i++){
        devices().push_back(_devs[i]);
    }
}
context::~context(){

}

}
