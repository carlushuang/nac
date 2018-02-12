#ifndef NAC_CONTEXT_H
#define NAC_CONTEXT_H
#include <vector>
#include <mutex>

#include "compute_device.h"

namespace nac{

using _nac_context = class context;

class context{
public:
    //static context * get_context();
    context(compute_device * _dev);
    ~context();

    inline compute_device * & device() {return dev_;}
    inline const ompute_device * & device() const {return dev_;}

private:
    compute_device * dev_;

};

}

#endif
