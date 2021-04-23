#pragma once

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#include "hft/log_level.h"
#include <atomic>
#include <cassert>
#include <chrono>  //NOLINT
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

//////////////////////////////////////////////////////////////////
#define COLOR_None "\033[m"
#define COLOR_RedLight "\033[1;31m"
#define COLOR_GreenLight "\033[1;32m"
#define COLOR_Yellow "\033[1;33m"

//////////////////////////////////////////////////////////////////
#define LOG_WAIT_TIMEOUT 5
#define TIMEVAL_MULTI 1000000
#define NPS 0.00001

// 尽量避免跳转语句
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

//////////////////////////////////////////////////////////////////
// gcc warning临时关闭
#if (__GNUC__ <= 4)
//# gcc4
#define GCC_IGNORE_OPSTR_TRUNC
#define GCC_IGNORE_OPSTR_OVERFLOW
#define GCC_IGNORE_MEMACCESS
#define GCC_IGNORE_DONE
#define GCC_OLD
//!
#else
//# gcc7或更高
#define GCC_IGNORE_OPSTR_TRUNC                                                                                         \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wstringop-truncation\"")

#define GCC_IGNORE_OPSTR_OVERFLOW                                                                                      \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wstringop-overflow\"")

#define GCC_IGNORE_MEMACCESS _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wclass-memaccess\"")

#define GCC_IGNORE_DONE _Pragma("GCC diagnostic pop")
//!
#endif

// math

namespace cro = std::chrono;

using std::atomic_bool;
using std::atomic_char;
using std::atomic_int;
using std::atomic_uint;
using std::atomic_ushort;
#ifdef GCC_OLD
typedef std::atomic<uint64_t> atomic_uint64_t;
#else
using std::atomic_uint64_t;
#endif

typedef std::atomic<uint8_t> atomic_uint8;
typedef std::atomic<uint16_t> atomic_uint16;
typedef std::atomic<double> atomic_double;
typedef std::atomic<int64_t> atomic_int64;
typedef std::atomic<uint64_t> atomic_uint64;

typedef unsigned char byte;

typedef cro::duration<int> dura_sec;
typedef cro::duration<int, std::ratio<60>> dura_min;
typedef cro::duration<int, std::ratio<60 * 60>> dura_hour;
typedef cro::duration<int, std::ratio<1, 1000>> dura_milli;
typedef cro::duration<int, std::ratio<1, 1000000>> dura_micro;
typedef cro::duration<int, std::ratio<1, 1000000000>> dura_nano;

// system clock accuracy is nano second
typedef cro::system_clock csc;
typedef csc::time_point csc_tp;
typedef csc::duration csc_dura;

namespace hft {
//////////////////////////////////////////////////////////////////
inline std::string Today() {
    time_t tt = csc::to_time_t(csc::now());
    struct tm t;
    char buffer[20] = {0};
#ifndef MINGW
    localtime_r(&tt, &t);
#else
    localtime_s(&t, &tt);
#endif
    strftime(buffer, sizeof(buffer), "%Y%m%d", &t);
    return buffer;
}
inline int Weekday() {
    time_t tt = csc::to_time_t(csc::now());
    struct tm t;
#ifndef MINGW
    localtime_r(&tt, &t);
#else
    localtime_s(&t, &tt);
#endif
    return t.tm_wday;
}
// 返回当前日期是这一年的第几天
inline int DaysInYear(const std::string& date) {
    int year = atoi(date.substr(0, 4).c_str()) - 1900;
    int month = atoi(date.substr(4, 2).c_str()) - 1;
    int day = atoi(date.substr(6, 2).c_str());
    // printf("%d %d %d", year, month, day);
    int days = 0;
    int str[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
        str[1] = 29;
    }
    for (int i = 0; i < month; ++i) {
        days += str[i];
    }
    return days + day;
}

// struct tm转时间戳(追求性能)
inline time_t time_to_epoch(const struct tm* ltm, int utcdiff = 0) {
    const int mon_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int32_t tyears, tdays, leaps, utc_hrs;
    int i;

    tyears = ltm->tm_year - 70;  // tm->tm_year is from 1900.
    // no of next two lines until year 2100.
    // @note: 2100年后会有问题，注意处理
    leaps = (tyears + 2) / 4;
    // i = (ltm->tm_year – 100) / 100;
    // leaps -= ( (i/4)*3 + i%4 );

    tdays = 0;
    for (i = 0; i < ltm->tm_mon; i++)
        tdays += mon_days[i];

    tdays += ltm->tm_mday - 1;  // days of month passed.
    tdays = tdays + (tyears * 365) + leaps;

    utc_hrs = ltm->tm_hour - utcdiff;  // for your time zone.
    // printf("ltm %ld %ld %d %d\n", tdays, utc_hrs, ltm->tm_min, ltm->tm_sec);
    return (tdays * 86400) + (utc_hrs * 3600) + (ltm->tm_min * 60) + ltm->tm_sec;
}
// 时间戳转换成本地系统时间
inline std::string TimesecToString(time_t tv_sec, const char* format = "%Y%m%d %H:%M:%S") {
    char buff[128] = {0};
    struct tm nowtm;
#ifndef MINGW
    localtime_r(&tv_sec, &nowtm);
#else
    localtime_s(&nowtm, &tv_sec);
#endif
    strftime(buff, sizeof(buff), format, &nowtm);
    return buff;
}

inline std::string TimevalToString(const timeval& tv, const char* format = "%Y%m%d %H:%M:%S") {
    return TimesecToString(tv.tv_sec, format);
}

// 简单返回errno的错误描述
inline std::string BuildError(const std::string& error = "") { return error + strerror(errno); }
// 获取当前目录
// 字符串根据分割符分割成vector
inline void Split(const std::string& s, std::vector<std::string>* list, const std::string& delim = ",") {
    size_t pos = 0;
    size_t len = s.length();
    size_t delim_len = delim.length();

    if (delim_len == 0) return;

    std::vector<std::string>& elems = *list;
    while (pos < len) {
        int find_pos = s.find(delim, pos);
        if (find_pos < 0) {
            elems.push_back(s.substr(pos, len - pos));
            break;
        }
        elems.push_back(s.substr(pos, find_pos - pos));
        pos = find_pos + delim_len;
    }
}
// 目录操作
inline std::string ExpandUser(const std::string& path) {
    std::string tmp = path;
    if (path[0] == '~') {
        const char* home = getenv("HOME");
        tmp = std::string(home) + path.substr(1, path.size() - 1);
    }
    return tmp;
}

//////////////////////////////////////////////////////////////////
// chrono handle
inline uint64_t CroCount(const csc_tp& t) { return t.time_since_epoch().count(); }
// return seconds, like timeval
inline uint32_t CroSec(const csc_tp& t) {
    // return (uint32_t)(t.time_since_epoch().count() * 0.000000001);
    return cro::duration_cast<cro::seconds>(t.time_since_epoch()).count();
}
// return nano seconds
inline uint32_t CroNanos(const csc_tp& t) {
    uint32_t count = t.time_since_epoch().count();
    count -= CroSec(t) * 1000000000;
    return count;
}
// return micro seconds
inline uint32_t CroUsec(const csc_tp& t) {
    // uint32_t count = (uint64_t)(CroNanos(t)/1000);
    uint32_t count = (uint32_t)(CroNanos(t) * 0.001);
    return count;
}
// return nano diff
inline int CroDiff(const csc_tp& t2, const csc_tp& t1) {
    dura_nano dura = cro::duration_cast<dura_nano>(t2 - t1);
    return dura.count();
}

// return micro diff
inline int CroDiffMicro(const csc_tp& t2, const csc_tp& t1) {
    dura_micro dura = cro::duration_cast<dura_micro>(t2 - t1);
    return dura.count();
}
inline int CroDiffMilli(const csc_tp& t2, const csc_tp& t1) {
    dura_milli dura = cro::duration_cast<dura_milli>(t2 - t1);
    return dura.count();
}
inline int CroDiffSec(const csc_tp& t2, const csc_tp& t1) {
    dura_sec dura = cro::duration_cast<dura_sec>(t2 - t1);
    return dura.count();
}

inline csc_tp ConvertTime(const timeval& tv) {
    auto d = cro::seconds{tv.tv_sec} + cro::microseconds{tv.tv_usec};
    return csc_tp(cro::duration_cast<csc::duration>(d));
}
inline std::string CroTime(const csc_tp& t) {
    size_t sec = CroSec(t);
    size_t usec = CroUsec(t);
    char buff[255] = {0};
    snprintf(buff, sizeof(buff), "%zu %06zu", sec, usec);
    return buff;
}

//////////////////////////////////////////////////////////////////
inline int64_t TimevalDiff(const timeval& t2, const timeval& t1) {
    int64_t usec = (t2.tv_sec - t1.tv_sec) * TIMEVAL_MULTI;
    return usec + t2.tv_usec - t1.tv_usec;
}
inline int64_t TimevalDiffSec(const timeval& t2, const timeval& t1) { return (t2.tv_sec - t1.tv_sec); }
inline void TimevalAdd(const timeval& t1, const timeval& delta, timeval& t2) {
    t2.tv_sec = t1.tv_sec + delta.tv_sec;
    if (t1.tv_usec + delta.tv_usec > TIMEVAL_MULTI) {
        t2.tv_sec += 1;
        t2.tv_usec = t1.tv_usec + delta.tv_usec - TIMEVAL_MULTI;
    } else {
        t2.tv_usec = t1.tv_usec + delta.tv_usec;
    }
    while (t2.tv_usec < 0) {
        t2.tv_usec += TIMEVAL_MULTI;
        t2.tv_sec -= 1;
    }
}
// 纳秒级sleep
inline int NanoSleep(uint32_t ns) {
    if (ns >= 1000000000) {
        char msg[255] = {0};
        snprintf(msg, sizeof(msg), "NanoSleep() use wrong nanosecond:%u", ns);
        throw std::runtime_error(msg);
    }
#ifndef MINGW
    timespec ts = {0, ns};
#else
    timespec ts = {0, static_cast<int64>(ns)};
#endif
    return nanosleep(&ts, NULL);
}
// 检测整数是否2^n
inline bool Is2Power(int num) { return num > 0 ? ((num & (~num + 1)) == num) : false; }
inline bool Is2Power(uint32_t num) { return ((num & (~num + 1)) == num); }
inline bool Is2Power(uint64_t num) { return ((num & (~num + 1)) == num); }
// double或float 比较大小
template <typename T1, typename T2>
inline int Compare(const T1& a, const T2& b) {
    if (fabs(a - b) < NPS) {
        return 0;
    } else if (a > b) {
        return 1;
    } else {
        return -1;
    }
}

//////////////////////////////////////////////////////////////////
// 快速转换(性能比stl快)
inline int fast_atoi(const char* str) {
    int val = 0;
    int multi = 1;
    while (*str) {
        if (LIKELY(*str >= '0' && *str <= '9')) {
            val = val * 10 + (*str++ - '0');
        } else if (*str == '-') {
            multi = -1;
            ++str;
        } else if (*str == ' ') {
            ++str;
            continue;
        } else {
            break;
        }
    }
    return val * multi;
}
inline int fast_atoi(const char* str, int cnt) {
    int val = 0;
    int multi = 1;
    for (int i = 0; i < cnt; ++i) {
        if (LIKELY(*str >= '0' && *str <= '9')) {
            val = val * 10 + (*str++ - '0');
        } else if (*str == '-') {
            multi = -1;
            ++str;
        } else if (*str == ' ') {
            ++str;
            continue;
        } else {
            break;
        }
    }
    return val * multi;
}
inline double fast_atof(const char* p) {
#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')
    int frac;
    double sign, value, scale;
    // Skip leading white space, if any.
    while (white_space(*p)) {
        p += 1;
    }
    // Get sign, if any.
    sign = 1.0;
    if (*p == '-') {
        sign = -1.0;
        p += 1;
    } else if (*p == '+') {
        p += 1;
    }

    // Get digits before decimal point or exponent, if any.
    for (value = 0.0; valid_digit(*p); p += 1) {
        value = value * 10.0 + (*p - '0');
    }
    // Get digits after decimal point, if any.
    if (*p == '.') {
        double pow10 = 10.0;
        p += 1;
        while (valid_digit(*p)) {
            value += (*p - '0') / pow10;
            pow10 *= 10.0;
            p += 1;
        }
    }
    // Handle exponent, if any.
    frac = 0;
    scale = 1.0;
    if ((*p == 'e') || (*p == 'E')) {
        unsigned int expon;
        // Get sign of exponent, if any.
        p += 1;
        if (*p == '-') {
            frac = 1;
            p += 1;
        } else if (*p == '+') {
            p += 1;
        }

        // Get digits of exponent, if any.
        for (expon = 0; valid_digit(*p); p += 1) {
            expon = expon * 10 + (*p - '0');
        }
        if (expon > 308) expon = 308;
        // Calculate scaling factor.
        while (expon >= 50) {
            scale *= 1E50;
            expon -= 50;
        }
        while (expon >= 8) {
            scale *= 1E8;
            expon -= 8;
        }
        while (expon > 0) {
            scale *= 10.0;
            expon -= 1;
        }
    }
    // Return signed and scaled floating point result.
    return sign * (frac ? (value / scale) : (value * scale));
}

//////////////////////////////////////////////////////////////////
// 二分查询(从大到小排序)
/* 递归法性能不好
inline int BinarySearchHL(std::vector<double> &a, double x, int left, int right) {
    if (left > right)
        return -1;

    int n = left;
    if (left != right) {
        n = (left + right) / 2;
    }
    int cmp = Compare(x, a[n]);
    if (cmp == 0)
        return n;
    else if (cmp > 0)
        return BinarySearchHL(a, x, left, n-1);
    else
        return BinarySearchHL(a, x, n+1, right);
}*/
inline int BinarySearchHL(std::vector<double>& a, double x, int left, int right) {
    int n = 0;
    while (left <= right) {
        n = left;
        if (left != right) {
            n = (left + right) / 2;
        }
        int cmp = Compare(x, a[n]);
        if (cmp == 0)
            return n;
        else if (cmp > 0)
            right = n - 1;
        else
            left = n + 1;
    }
    return -1;
    // return n;
}

template <class T>
void Swap(T& a, T& b, T& tmp) {
    tmp = std::move(a);
    a = std::move(b);
    b = std::move(tmp);
}

// 将float或double类型转化为int类型
// 采取四舍五入的方式，对小数位后7位四舍五入后，round之转化为int
#define MAX_NUM_FOR_ROUND 10000000
template <typename T>
int Round(const T& val) {
    return round(round(val * MAX_NUM_FOR_ROUND) / MAX_NUM_FOR_ROUND);
}
template <typename T>
int Floor(const T& val) {
    return floor(round(val * MAX_NUM_FOR_ROUND) / MAX_NUM_FOR_ROUND);
}
template <typename T>
int Ceil(const T& val) {
    return Ceil(round(val * MAX_NUM_FOR_ROUND) / MAX_NUM_FOR_ROUND);
}
}  // namespace hft
