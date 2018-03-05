#if defined(_NAC_INCLUDE_HYPERPARAMETER_DETAIL)

class activation_hparam : public hyperparameter{
public:
    virtual void map(hparam_map * pmap){
        act_type_ = hparam_to_int(pmap->find_param("activation"));
    }
private:
    virtual void outsize(int * out_w, int * out_h, int * out_c, int * out_n){
    }
    NAC_RW_ATTR(activation_type, act_type)
};

#endif
