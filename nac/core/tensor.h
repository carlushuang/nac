#ifndef NAC_TENSOR_H
#define NAC_TENSOR_H

namespace nac{

class tensor{
public:
    tensor(int _w, int _h, int _c, int _n, void * _ptr) : w_(_w), h_(_h), c_(_c), n_(n_), data_(_ptr) {}
    tensor(int _w, int _h, int _c, int _n) : tensor(_w, _h, _c, _n, nullptr){}
    tensor(int _size, void * _ptr) : w_(_size), h_(1), c_(1), n_(1), data_(_ptr){}
    tensor() : tensor(0, 0, 0, 0){}

    inline void * & data() { return data_;}
    inline void * data() const{ return data_;}

    inline int & w() { return w_;}
    inline int w() const { return w_;}

    inline int & h() { return h_;}
    inline int h() const { return h_;}

    inline int & c() { return c_;}
    inline int c() const { return c_;}

    inline int & n() { return n_;}
    inline int n() const { return n_;}

    inline int size() const { return w_*h_*c_*n_;}

    inline bool empty() const {return data_==nullptr;}
private:
    int w_, h_, c_, n_;
    void * data_;
};

}

#endif