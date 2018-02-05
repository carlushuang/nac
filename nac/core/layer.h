#ifndef NAC_LAYER_H
#define NAC_LAYER_H

#include "context.h"
#include "operator_base.h"
#include "observable.h"
#include "layer_attribute.h"

namespace nac{

class layer : public observable{
public:
    virtual int load_attribute(layer_attribute *) = 0;
    virtual int load_weight(void *) = 0;

protected:
    context * ctx;
    operator_base * op;
    layer_attribute * attr;
};

}

#endif