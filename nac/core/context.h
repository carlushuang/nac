#ifndef NAC_CONTEXT_H
#define NAC_CONTEXT_H

#include "common.h"
#include <vector>
#include <mutex>

namespace nac{

class compute_device;

class context{
public:
    //static context * get_context();
    context(compute_device ** _devs, int _num_devs);
    ~context();

NAC_RW_ATTR(std::vector<compute_device *>, devices)
};

using _nac_context = context;

}

#endif
