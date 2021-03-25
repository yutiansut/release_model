#pragma once

#include <vector>
#include <cmath>

namespace math_util {
    template<typename T>
    HUSKY_INLINE T sum(const std::vector<T> &vec) {
        T ret;
        for (const auto &t : vec) ret += t;
        return ret;
    }

    template<typename T>
    HUSKY_INLINE double avg(const std::vector<T> &vec) {
        return (double) sum(vec) / vec.size();
    }

    template<typename T>
    HUSKY_INLINE double nan_check(T t) {
        return std::isnan(t) ? 0 : t;
    }

    template<typename T>
    HUSKY_INLINE bool is_nan(T t) {
        return std::isnan(t);
    }

    template<typename T>
    HUSKY_INLINE bool are_same(T a, T b, double epsilon) {
        return fabs(a - b) < epsilon;
    }

    // -1 0
    // 0 1
    // 1.00001 1.00000
    template<typename T>
    HUSKY_INLINE bool f_lt(T a, T b, double epsilon) {
        return fabs(b - a) < epsilon ? false : a < b;
    }

    template<typename T>
    HUSKY_INLINE bool f_gt(T a, T b, double epsilon) {
        return fabs(b - a) < epsilon ? false : a > b;
    }

    template<typename T>
    HUSKY_INLINE bool f_lte(T a, T b, double epsilon) {
        return a < b || fabs(b - a) < epsilon;
    }

    template<typename T>
    HUSKY_INLINE bool f_gte(T a, T b, double epsilon) {
        return a > b || fabs(b - a) < epsilon;
    }

    template<typename T>
    HUSKY_INLINE bool f_eq(T a, T b, double epsilon) {
        return are_same(a, b, epsilon);
    }
}