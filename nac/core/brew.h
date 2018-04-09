#ifndef __BREW_H
#define __BREW_H

#include "args_map.h"
#include <memory>

namespace nac{

class brew{
public:
    brew(args_map * args){
        args_.reset(args);
    }
    virtual ~brew(){}

protected:
    std::unique_ptr<args_map> args_;
};

}

#if NAC_BREW_BACKEND_DARKNET
#include "brew/brew_darknet.h"
#endif

#endif
