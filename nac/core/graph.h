#ifndef NAC_GRAPH_H
#define NAC_GRAPH_H

#include "observable.h"
#include "context.h"
#include "node.h"
#include "tensor.h"
#include "common.h"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>

namespace nac{

using _nac_graph = class graph;

class graph : public observable{
public:
    graph(context * _ctx) :ctx(_ctx), execution_thread(nullptr), execution_need_exit(false),
        in_execution(false)
        {}
    virtual ~graph(){
        if(execution_thread){
            execution_need_exit = true;
            execution_thread->join();
            delete execution_thread;
            in_execution = false;
        }
        release_nodes();
        release_inputs();
    }
    inline bool is_executing() const{
        return in_execution.load();
    }
    inline unsigned int loops() const{
        return loop_cnt.load();
    }

    inline void start_inference(int loop, bool need_wait){
        if(need_wait){
            in_execution = true;
            for(int i=0;i<loop;i++){
                inference_once();
            }
            in_execution = false;
        }else {
            if(!execution_thread && !in_execution){
                in_execution = true;
                execution_need_exit = false;
                execution_thread = new std::thread([&, this](){
                    if(loop<=0){
                        while(!execution_need_exit) // give a chance that infinit loop can stop
                            inference_once();
                    }else{
                        while(!execution_need_exit && loop--)
                            inference_once();
                    }
                });
            }
        }
    }


    inline void wait(){
        // block for execution finish
        if(execution_thread && in_execution){
            execution_thread->join();
            delete execution_thread;
            in_execution = false;
        }
    }

#if 0
    inline void inference_loop(int loop){
        in_execution = true;
        for(int i=0;i<loop;i++){
            inference_once();
        }
        in_execution = false;
    }
#endif
    inline void attach_node(node * n){
        if(in_execution)
            return;
        nodes.emplace_back(n);
    }
    inline void attach_nodes(node ** n, int num){
        if(in_execution)
            return;
        for(int i=0;i<num;i++){
            nodes.emplace_back(n[i]);
        }
    }
    inline void attach_nodes(std::vector<node*> & n_vec){
        if(in_execution)
            return;
        for(auto & n : n_vec){
            nodes.emplace_back(n);
        }
    }
    inline void release_nodes(){
        if(in_execution)
            return;
        for(auto it = nodes.begin(); it != nodes.end();){
            it = nodes.erase(it);
        }
    }
    inline void release_inputs(){
        if(in_execution)
            return;
        for(auto it = inputs.begin(); it != inputs.end();){
            it = inputs.erase(it);
        }
    }

    inline void feed_input(tensor * t){
        inputs.emplace_back(t);
    }
    inline void feed_inputs(tensor ** t, int num){
        for(int i=0;i<num;i++){
            inputs.emplace_back(t[i]);
        }
    }
    inline void feed_inputs(std::vector<tensor*> & t_vec){
        for(auto & t : t_vec){
            inputs.emplace_back(t);
        }
    }

    inline tensor * output(int idx = 0) const{
        // TODO: aync query output
        // assume output is from the last of nodes
        if(nodes.empty()){
            NAC_ERROR("no op node in current graph, get the output is not valid");
            return nullptr;
        }
        NAC_ASSERT(idx<nodes.size(), "request idx:", idx, " is bigger than total nodes number", nodes.size());
        return nodes.at(idx)->output(idx);;
    }

private:

    void inference_once(){
        start_observe();
        for(auto & n : nodes)
            n->forward();

        loop_cnt++;
        stop_observe();
    }


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