#ifndef NAC_EXECUTOR_H
#define NAC_EXECUTOR_H


#include "graph"
#include <memory>
#include <vector>

namespace nac{

class executor{
public:

private:
    std::vector<std::unique_ptr<graph>>   graphs_;
};

}

#endif