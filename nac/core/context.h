#ifndef NAC_CONTEXT_H
#define NAC_CONTEXT_H
#include <vector>
#include <mutex>

#include "compute_device.h"
#include <vector>

namespace nac{

using _nac_context = class context;

class context{
public:
    //static context * get_context();
    context(compute_device ** _devs, int _num_devs);
    ~context();

NAC_RW_ATTR(std::vector<compute_device *>, devices)
};

}

#endif
