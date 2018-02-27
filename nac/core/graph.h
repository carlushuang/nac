#ifndef NAC_GRAPH_H
#define NAC_GRAPH_H

#include "observable.h"
#include "context.h"
#include "node.h"
#include "tensor.h"
#include <vector>
#include <memory>

namespace nac{

class graph : public observable{
public:
    void start_forward();
private:
    context * ctx;
    std::vector<std::unique_ptr<node>> nodes;
#if 0
    struct node_store{
        std::unique_ptr<node>                   ptr;
        std::vector<std::unique_ptr<tensor>>    output;
        std::vector<std::unique_ptr<tensor>>    weight;
    };
    std::vector<node_store> nodes;
#endif
    std::vector<std::unique_ptr<tensor>> inputs;        // graph inputs


DISABLE_COPY_AND_ASSIGN(graph)
};

}


#endif