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
    context();
    ~context();

private:
    

    void init_once();

    void probe_compute_device();


    static std::mutex dev_lock;
    static std::vector<unique_ptr<compute_device>> dev_list;
};

}

#endif