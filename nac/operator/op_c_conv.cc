#include "op_c_registry.h"

#include <operator_base.h>


namespace nac{
    
class op_c_conv : public operator_base{



};



NAC_OP_REGISTER(c, conv, op_c_conv)
}