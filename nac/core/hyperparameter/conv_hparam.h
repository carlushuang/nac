#if defined(_NAC_INCLUDE_HYPERPARAMETER_DETAIL)

class conv_hparam : public hyperparameter{
    NAC_RW_ATTR(int, kernel)
    NAC_RW_ATTR(int, padding)
    NAC_RW_ATTR(int, stride)
    NAC_RW_ATTR(int, filters)
    NAC_RW_ATTR(activation_type, act_type)
    // NAC_RW_ATTR(int, dilation)
    // NAC_RW_ATTR(int, group)
};

#endif