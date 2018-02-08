#ifndef NAC_HYPERPARAMETER_H
#define NAC_HYPERPARAMETER_H

#include "common.h"

namespace nac{

class hyperparameter{
    NAC_RW_ATTR(int, batch)
};

#define  _NAC_INCLUDE_HYPERPARAMETER_DETAIL
#include "hyperparameter/conv_hparam.h"
#include "hyperparameter/activation_hparam.h"
#include "hyperparameter/softmax_hparam.h"

}

#endif