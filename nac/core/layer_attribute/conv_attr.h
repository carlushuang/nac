#ifndef NAC_CONV_ATTR_H
#define NAC_CONV_ATTR_H

#include "../layer_attribute.h"

class conv_attr : public layer_attribute{
public:
    int kernel;
    int padding;
    int stride;
};

#endif