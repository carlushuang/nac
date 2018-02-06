#ifndef NAC_OBSERVER_H
#define NAC_OBSERVER_H

#include "observable.h"

namespace nac{

class observer{
public:
    observer():target_(nullptr){}
    virtual ~observer(){}

    observable *& target() { return target_;}
    const observable *& target() const { return target_;}

    virtual start(){}
    virtual stop(){}
private:
    observable * target_;
};

}

#endif