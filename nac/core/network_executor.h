#ifndef NAC_NETWORK_EXECUTOR_H
#define NAC_NETWORK_EXECUTOR_H

#include "context.h"
#include "network.h"
#include <vector>

namespace nac{

class network_executor{
public:

    virtual int forward_network(){}

protected:
    context                *  ctx;
    network                *  net;
};

}

#endif
