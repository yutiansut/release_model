#pragma once
//#include "base/util.h"
#include <string>
namespace hft {
const char* SFormat(const std::string& i) { return i.c_str(); }
const char* SFormat(const int& i) { return StringFormat("%d", i).c_str(); }
const char* SFormat(const unsigned int& i) { return StringFormat("%ud", i).c_str(); }
const char* SFormat(const long& i) { return StringFormat("%ld", i).c_str(); }
// const char* SFormat(const unsigned long& i) { return StringFormat("%uld", i).c_str(); }
const char* SFormat(const size_t& i) { return StringFormat("%lud", i).c_str(); }
const char* SFormat(const double& i) { return StringFormat("%g", i).c_str(); }
}  // namespace hft
