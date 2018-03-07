#ifndef NAC_GRAPH_H
#define NAC_GRAPH_H

#include "observable.h"
#include "common.h"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>

namespace nac{

class context;
class node;
class tensor;

class graph : public observable{
public:
    graph(context * _ctx);
    virtual ~graph();
    inline bool is_executing() const{
        return in_execution.load();
    }
    inline unsigned int loops() const{
        return loop_cnt.load();
    }

    void start_inference(int loop, bool need_wait);
    void wait();

#if 0
    inline void inference_loop(int loop){
        in_execution = true;
        for(int i=0;i<loop;i++){
            inference_once();
        }
        in_execution = false;
    }
#endif
    void attach_node(node * n);
    void attach_nodes(node ** n, int num);
    void attach_nodes(std::vector<node*> & n_vec);

    void release_nodes();
    void release_inputs();

    void feed_input(tensor * t);
    void feed_inputs(tensor ** t, int num);
    void feed_inputs(std::vector<tensor*> & t_vec);

    tensor * output(int idx = 0) const;

private:

    void inference_once();

    context * ctx;
    std::vector<std::unique_ptr<node>> nodes;
    std::atomic_bool    in_execution;
    std::atomic_uint    loop_cnt;

    std::vector<std::unique_ptr<tensor>> inputs;        // graph inputs
    std::thread * execution_thread;
    std::atomic_bool    execution_need_exit;

DISABLE_COPY_AND_ASSIGN(graph)
};


}


#endif