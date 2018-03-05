#if defined(_NAC_INCLUDE_HYPERPARAMETER_DETAIL)

class maxpool_hparam : public hyperparameter{
public:
    virtual void map(hparam_map * pmap){
        kernel_ = hparam_to_int(pmap->find_param("kernel"));
        padding_ = hparam_to_int(pmap->find_param("padding"));
        stride_ = hparam_to_int(pmap->find_param("stride"));
    }
private:
    virtual void outsize(int * out_w, int * out_h, int * out_c, int * out_n){
    }
    NAC_RW_ATTR(int, kernel)
    NAC_RW_ATTR(int, padding)
    NAC_RW_ATTR(int, stride)
};

#endif
