#ifndef _HUSKY_COMPACT_HELPER_H_
#define _HUSKY_COMPACT_HELPER_H_

#include <string>

template<typename N>
N parseStr(const std::string& s);

template<> inline int parseStr<int>(const std::string& s) {
    return std::stoi(s);
}

template<> inline float parseStr<float>(const std::string& s) {
    return std::stof(s);
}

template<> inline double parseStr<double>(const std::string& s) {
    return std::stod(s);
}

template<> inline std::string parseStr<std::string>(const std::string& s) {
    return s;
}

template<> inline uint32_t parseStr<uint32_t>(const std::string& s) {
    return std::stoul(s);
}

template<> inline unsigned long parseStr<uint64_t>(const std::string& s) {
    return std::stoull(s);
}

// only compact with little endian cpu
// if integer < 0, result = 0
// if integer >=0, result = 1
#define INT8_EXTRACT_SIGN(u)  int8_t(((0x80 & u)               ^ 0x80)               >> 7 )
#define INT16_EXTRACT_SIGN(u) int8_t(((0x8000 & u)             ^ 0x8000)             >> 15)
#define INT32_EXTRACT_SIGN(u) int8_t(((0x80000000 & u)         ^ 0x80000000)         >> 31)
#define INT64_EXTRACT_SIGN(u) int8_t(((0x8000000000000000 & u) ^ 0x8000000000000000) >> 63)



#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)


namespace husky {

template<bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

}
#endif
