#if defined(_NAC_INCLUDE_HYPERPARAMETER_DETAIL)

class softmax_hparam : public hyperparameter{
public:
    virtual void map(hparam_map * pmap){
        temperature_ = pmap->find_int("temperature", 0);
        axis_ = pmap->find_int("axis", 0);
    }
private:
    virtual void outsize(int * out_w, int * out_h, int * out_c, int * out_n){
    }
    NAC_RW_ATTR(int, temperature)
    NAC_RW_ATTR(int, axis)      // refer to tensorflow

};

#endif
