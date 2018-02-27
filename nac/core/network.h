#ifndef NAC_NETWORK_H
#define NAC_NETWORK_H

#include "network_attribute.h"
#include "observable.h"
#include "context.h"
#include "node.h"
#include <vector>

namespace nac{

class network : public observable{
public:
    void start_forward();
private:
    network_attribute * attr;
    context * ctx;
    std::vector<node*> layers;
};

}

#endif