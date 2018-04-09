#include "brew_darknet.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <common.h>

namespace nac{

static std::string& ltrim(std::string& s) {
    size_t startpos = s.find_first_not_of(" \t\r\n\v\f");
    if (std::string::npos != startpos)
    {
        s = s.substr(startpos);
    }
    return s;
}

static std::string& rtrim(std::string& s) {
    size_t endpos = s.find_last_not_of(" \t\r\n\v\f");
    if (std::string::npos != endpos)
    {
        s = s.substr(0, endpos + 1);
    }
    return s;
}

struct section{
    std::string name;
    args_map    store;
};
struct config{
public:
    config(const std::string & cfg_file){
        file_name = cfg_file;
        parse();
    }
    std::string             file_name;
    std::vector<section>    contents;
    void dump() const {
        for(auto & sec: contents){
            std::cout<<"["<<sec.name<<"]"<<std::endl;
            for(auto & kv : sec.store){
                std::cout<<kv.first<<"="<<kv.second<<std::endl;
            }
            std::cout<<"\n";
        }
    }
private:
    void parse(){
        section currentsection;
        std::ifstream fstrm;
        fstrm.open(file_name);
        if(!fstrm){
            NAC_ASSERT(0, "can not open cfg file ", file_name);
        }

        for (std::string line; std::getline(fstrm, line);  )
        {
            if (!line.empty() && (line[0] == ';' || line[0] == '#')) {
                // comment
                continue;
            }
            if (line[0] == '[') {
                /* A "[section]" line */
                size_t end = line.find_first_of(']');
                if (end != std::string::npos) {
                    if (!currentsection.name.empty()) {
                        contents.push_back(currentsection);  // copy
                        currentsection.name.clear();  // clear section for re-use
                        currentsection.store.clear();
                    }
                    currentsection.name = line.substr(1, end - 1);
                }
                else {
                    // section has no closing ] char
                    NAC_WARNING("no ending \"]\" found in this line:\"", line, "\"");
                }
            }
            else if (!line.empty()) {
                /* Not a comment, must be a name[=:]value pair */
                size_t end = line.find_first_of("=:");
                if (end != std::string::npos) {
                    std::string name = line.substr(0, end);
                    std::string value = line.substr(end + 1);
                    ltrim(rtrim(name));
                    ltrim(rtrim(value));

                    currentsection.store.insert(name, value);
                }
                else {
                    // no key value delimitter
                }
            }

            if (!currentsection.name.empty()) {
                contents.push_back(currentsection);  // copy
                currentsection.name = "";
                currentsection.store.clear();
            }
        }
    }
};

brew_darknet::brew_darknet(args_map * args):brew(args){
    // may terminate if not found
    cfg_file = args_->find(ARG_DARKNET_CFG_FILE);
    weight_file = args->find(ARG_DARKNET_WEIGHT_FILE);

    config cfg(cfg_file);
    cfg.dump();
}
brew_darknet::~brew_darknet(){

}

}
