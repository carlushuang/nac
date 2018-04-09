#ifndef __BREW_DARKNET_H
#define __BREW_DARKNET_H

#include <brew.h>
#include <string>

#define ARG_DARKNET_CFG_FILE        "cfg_file"
#define ARG_DARKNET_WEIGHT_FILE     "weight_file"

namespace nac{

class brew_darknet : public brew
{
public:
    brew_darknet(args_map * args);
    ~brew_darknet();
private:
    std::string cfg_file;
    std::string weight_file;
};

}
#endif
