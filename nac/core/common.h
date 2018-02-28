#ifndef NAC_COMMON_H
#define NAC_COMMON_H

#include <sstream>
#include <string>
#include <exception>
#include <iostream>

#define NAC_DATA_FP32   0x0
#define NAC_DATA_FP16   0x1
#define NAC_DATA_MAX    4


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


//#define NAC_OP_NAME(entry_name, data_type, op_name)  \
//        #entry_name "-" #data_type "-"  #op_name 
inline std::string nac_op_name(std::string entry_name, std::string data_type, std::string bare_op_name){
    return entry_name+"-"+data_type+"-"+bare_op_name;
}

inline const char * data_type_to_str(int data_type){
    if(data_type == NAC_DATA_FP32)
        return "fp32";
    if(data_type == NAC_DATA_FP16)
        return "fp16";

    return "none";
}
inline int str_to_data_type(const char * str){
    std::string s(str);
    if(s == "fp32")
        return NAC_DATA_FP32;
    if(s == "fp16")
        return NAC_DATA_FP16;

    return NAC_DATA_MAX;
}

}

#ifdef NDEBUG
#define NAC_ASSERT(cond, msg)
#else
#define NAC_ASSERT(cond, ... ) \
    do{ \
        if( !(cond)) { \
            std::cerr<<"[nac] terminate due to assert in "<<__FILE__<<", line:"<<__LINE__ \
                <<"\n  \""<<#cond<<"\", \""<<nac::make_string(__VA_ARGS__)<< "\""<<  std::endl; \
            std::terminate(); \
        }\
    } \
    while(0)
#endif

#define NAC_WARNING(...) \
    do{\
        std::cerr<<"[nac] warning in "<<__FILE__<<", line:"<<__LINE__ \
            <<"\n  \""<<nac::make_string(__VA_ARGS__)<<"\""<<  std::endl; \
    }while(0)

#define NAC_ERROR(...) \
    do{\
        std::cerr<<"[nac] error in "<<__FILE__<<", line:"<<__LINE__ \
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


#define DISABLE_COPY_AND_ASSIGN(classname)  \
    private:   \
        classname(const classname&) = delete;  \
        classname& operator=(const classname&) = delete;

#endif