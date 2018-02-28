#if defined(_NAC_INCLUDE_HYPERPARAMETER_DETAIL)

class maxpool_hparam : public hyperparameter{
    virtual void do_calculate_outsize(int * out_w, int * out_h, int * out_c, int * out_n){
    }
    NAC_RW_ATTR(int, kernel)
    NAC_RW_ATTR(int, padding)
    NAC_RW_ATTR(int, stride)
};

#endif
