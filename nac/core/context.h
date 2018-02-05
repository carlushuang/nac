#ifndef NAC_CONTEXT_H
#define NAC_CONTEXT_H

namespace nac{

using _nac_context = class context;

class context{
public:
    static context * get_context();

private:
    context(){}
    ~context(){}

    void init_once();



    static context INSTANCE;
};

}

#endif