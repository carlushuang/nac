#ifndef NAC_OBSERVER_H
#define NAC_OBSERVER_H

namespace nac{

class observable;

class observer{
public:
    observer():target_(nullptr){}
    virtual ~observer(){}

    observable *& target() { return target_;}
    observable * const & target() const { return target_;}

    virtual void start(){}
    virtual void stop(){}
private:
    observable * target_;
};

}

#endif