#if defined(_NAC_INCLUDE_HYPERPARAMETER_DETAIL)

class maxpool_hparam : public hyperparameter{
    NAC_RW_ATTR(int, kernel)
    NAC_RW_ATTR(int, padding)
    NAC_RW_ATTR(int, stride)
};

#endif
