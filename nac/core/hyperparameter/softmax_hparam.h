#if defined(_NAC_INCLUDE_HYPERPARAMETER_DETAIL)

class softmax_hparam : public hyperparameter{
public:
    virtual void map(hparam_map * pmap){
        temperature_ = hparam_to_int(pmap->find_param("temperature"));
        axis_ = hparam_to_int(pmap->find_param("axis"));
    }
private:
    virtual void outsize(int * out_w, int * out_h, int * out_c, int * out_n){
    }
    NAC_RW_ATTR(int, temperature)
    NAC_RW_ATTR(int, axis)      // refer to tensorflow

};

#endif
