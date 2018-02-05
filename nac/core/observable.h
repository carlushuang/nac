#ifndef NAC_OBSERVABLE_H
#define NAC_OBSERVABLE_H

#include <vector>
#include <memory>
#include "observer.h"

namespace nac{

class observable{
public:
    observable(){}
    virtual ~observable(){}

    void attach(observer * ob){
        for(auto & ob_ptr : ob_list){
            if(ob_ptr.get() == ob)
                return ;
        }
        ob->target() = this;
        std::unique_ptr<observer> new_ob_ptr(ob);
        ob_list.push_back(std::move(new_ob_ptr));
    }
    void detach(const observer * ob){}

    void start_observe(){
        for(auto & ob_ptr:ob_list){
            ob_ptr.get()->start();
        }
    }
    void stop_observe(){
        for(auto & ob_ptr:ob_list){
            ob_ptr.get()->stop();
        }
    }

private:
    std::vector<std::unique_ptr<observer>> ob_list;
};

}

#endif