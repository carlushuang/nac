#if defined(_NAC_INCLUDE_HYPERPARAMETER_DETAIL)

class softmax_hparam : public hyperparameter{
    virtual void do_calculate_outsize(int * out_w, int * out_h, int * out_c, int * out_n){
    }
    NAC_RW_ATTR(int, temperature)
    NAC_RW_ATTR(int, axis)      // refer to tensorflow

};

#endif
