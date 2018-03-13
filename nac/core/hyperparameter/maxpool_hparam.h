#if defined(_NAC_INCLUDE_HYPERPARAMETER_DETAIL)

#define POOLING_OUTSIZE_TYPE_DEFAULT   0    // standard method
#define POOLING_OUTSIZE_TYPE_DARKNET   1
#define POOLING_OUTSIZE_TYPE_CAFFE     2

class maxpool_hparam : public hyperparameter{
public:
    virtual void map(hparam_map * pmap){
        kernel_ = hparam_to_int(pmap->find_param("kernel"));
        padding_ = hparam_to_int(pmap->find_param("padding"));
        stride_ = hparam_to_int(pmap->find_param("stride"));
        outsize_type_ = hparam_to_int(pmap->find_param("outsize_type", "0"));
    }
private:
    virtual void outsize(int * out_w, int * out_h, int * out_c, int * out_n){
        int w = op_node()->input()->w();
        int h = op_node()->input()->h();
        int c = op_node()->input()->c();
        int n = op_node()->input()->n();
        // pooling layer size calculation differ between frameworks
        // TODO: calculation method decide
        if(outsize_type() == POOLING_OUTSIZE_TYPE_DEFAULT){
            *out_w = (w - kernel_ + 2*padding_)/stride_ + 1;
            *out_h = (h - kernel_ + 2*padding_)/stride_ + 1;
            *out_c = c;
            *out_n = n;
        }else if(outsize_type() == POOLING_OUTSIZE_TYPE_DARKNET){
            *out_w = (w + 2*padding_)/stride_;
            *out_h = (h + 2*padding_)/stride_;
            *out_c = c;
            *out_n = n;
        }else if(outsize_type() == POOLING_OUTSIZE_TYPE_CAFFE){
            NAC_ASSERT(0, "caffe type need implement");
        }else{
            NAC_ASSERT(0, "no outsize cal type specified");
        }
    }
    NAC_RW_ATTR(int, kernel)
    NAC_RW_ATTR(int, padding)
    NAC_RW_ATTR(int, stride)
    // 0-darknet ver, 1-
    NAC_RW_ATTR(int, outsize_type)   // how to calculate out size
};

#endif
