#if defined(_NAC_INCLUDE_HYPERPARAMETER_DETAIL)

class softmax_hparam : public hyperparameter{
    NAC_RW_ATTR(int, temperature)
    NAC_RW_ATTR(int, axis)      // refer to tensorflow

};

#endif
