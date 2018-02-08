#ifndef NAC_INTERNAL_H
#define NAC_INTERNAL_H

#include <sstream>
#include <string>

namespace nac{

inline void make_string_variadic(std::stringstream& /*ss*/) { /*terminate ver*/}

template <typename T>
inline void make_string_variadic(std::stringstream& ss, const T& t) {
    ss << t;
}

template <typename T, typename... Args>
inline void make_string_variadic(std::stringstream& ss, const T& t, const Args&... args) {
    make_string_variadic(ss, t);
    make_string_variadic(ss, args...);
}
template <typename... Args>
inline std::string make_string(const Args&... args){
    std::stringstream ss;
    make_string_variadic(ss, args...);
    return std::string(ss.str());
}
template <>
inline std::string make_string(const std::string& str) {
  return str;
}
inline std::string make_string(const char* c_str) {
  return std::string(c_str);
}


enum class activation_type{
    LOGISTIC, RELU, RELIE, LINEAR, RAMP, TANH, PLSE, LEAKY, ELU, LOGGY, STAIR, HARDTAN, LHTAN
};

}

#ifdef NDEBUG
#undef NAC_ASSERT
#endif

#include <exception>
#include <iostream>
#ifdef NAC_ASSERT
#define NAC_ASSERT_COND(cond, ... ) \
    do{ \
        if( !(cond)) { \
            std::cerr<<"[nac] terminate due to assert in "<<__FILE__<<", line:"<<__LINE__ \
                <<"\n  \""<<#cond<<"\", \""<<nac::make_string(__VA_ARGS__)<< "\""<<  std::endl; \
            std::terminate(); \
        }\
    } \
    while(0)

#else
#define NAC_ASSERT_COND(cond, msg)
#endif

#define NAC_WARNING(...) \
    do{\
        std::cerr<<"[nac] warning in "<<__FILE__<<", line:"<<__LINE__ \
            <<"\n  \""<<nac::make_string(__VA_ARGS__)<<"\""<<  std::endl; \
    }while(0)


#define NAC_CONCAT__(a,b)   a ## b
#define NAC_CONCAT(a,b)     NAC_CONCAT__(a,b)


#define NAC_RW_ATTR(type, name) \
    private: \
        type name##_; \
    public:\
        inline type & name () {return this->name##_;} \
        inline type name () const {return this->name##_;}

#define NAC_R_ATTR(type, name) \
    private: \
        type name##_; \
    public:\
        inline type name () const {return this->name##_;}


#endif