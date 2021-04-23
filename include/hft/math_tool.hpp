#pragma once
#include "common/log.h"
#include <cmath>
#include <unordered_map>
// 函数
#define SIGN(x) ((0 < x) - (x < 0))
#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x > y ? y : x)
/*
 *  get_mean( itr_begin, itr_end)
 *  求[itr_begin, itr_end] 内的均值，包含itr_begin和itr_end
 * */
namespace math_tool {
template <typename TItr>
double get_mean(const TItr& _begin, const TItr& _end) {
    double sum{0.0};
    for (TItr itr = _begin; itr <= _end; ++itr) {
        sum += *itr;
    }
    return sum / (_end - _begin + 1);
}
// 在c++11中，返回类型不能使用auto， 但是在c++14中可以
template <typename TItr>
double get_sum(const TItr& _begin, const TItr& _end) {
    double sum{0.0};
    for (TItr itr = _begin; itr <= _end; ++itr) {
        sum += *itr;
    }
    return sum;
}
template <typename T>
int8_t sign(const T& val) {
    return (T(0) < val) - (val < T(0));
}

template <typename T1, typename T2>
typename std::common_type<T1, T2>::type max(const T1& a, const T2& b) {
    return a > b ? a : b;
}

template <typename T1, typename T2>
typename std::common_type<T1, T2>::type min(const T1& a, const T2& b) {
    return a < b ? a : b;
}

template <typename T>
double entropy(const T* _begin, const T* _end) {
    std::unordered_map<T, int> umap;
    for (auto itr = _begin; itr <= _end; ++itr) {
        if (umap.find(*itr) == umap.end()) {
            umap.insert({*itr, 0});
        } else {
            umap[*itr]++;
        }
    }
    return 0;
}
double entropy_fast(const int* _begin, const int* _end);

template <typename TItr1, typename TItr2, typename T>
void LinearFit(const TItr1& m_x_end, const TItr2& m_y_end, const int& length, T* out_slope) {
    // 对数据X，Y做一维的回归分析
    // 包含 x_end, y_end指针指向对象在内的数据，数据的总长度为length
    // 最好 TItr指针类型和T一致
    // 函数不对数据做任何检查，需要保证数据的正确性
    TItr1 x_end{m_x_end};
    TItr2 y_end{m_y_end};
    double m_slope{0};
    auto tmp_sum_x{*x_end};
    auto tmp_sum_xsq{(*x_end) * (*x_end)};
    auto tmp_sum_y{*y_end};
    auto tmp_sum_xy{(*x_end) * (*y_end)};
    --y_end;
    --x_end;
    for (int i = 0; i < length - 1; ++i, --x_end, --y_end) {
        tmp_sum_x += *x_end;
        tmp_sum_xsq += (*x_end) * (*x_end);
        tmp_sum_y += *y_end;
        tmp_sum_xy += (*x_end) * (*y_end);
        // printf("%g ", *x_end);
    }
    // out_slope的表达式中可能出现整数相除
    T w{1};
    w /= length;
    *out_slope = (tmp_sum_xy - w * tmp_sum_y * tmp_sum_x) / (tmp_sum_xsq - w * tmp_sum_x * tmp_sum_x);

    return;
}

template <typename TItr1, typename TItr2, typename T>
void Corr(const TItr1& x_end, const TItr2& y_end, const size_t& n, T* out_corr, const int& stride = 1) {
    // Corr(x, y) = E[(x - E[x])*(y - E[y])] / STD[x] STD[y]
    //        = E[xy] - E[x] E[y]
    // STD[x] 为 x 的标准差
    TItr1 x_itr{x_end};
    TItr2 y_itr{y_end};
    auto x_sum{*x_end};
    auto xsq_sum{(*x_end) * (*x_end)};
    auto y_sum{*y_end};
    auto ysq_sum{(*y_end) * (*y_end)};
    auto xy_sum{(*y_end) * (*x_end)};
    // T w{1.0 / n};
    for (uint16_t i = 1; i < n; ++i) {
        x_itr -= stride;
        y_itr -= stride;
        x_sum += *x_itr;
        xsq_sum += (*x_itr) * (*x_itr);
        y_sum += *y_itr;
        ysq_sum += (*y_itr) * (*y_itr);
        xy_sum += (*x_itr) * (*y_itr);
    }
    *out_corr = (n * xy_sum - x_sum * y_sum) / sqrt((n * xsq_sum - x_sum * x_sum) * (n * ysq_sum - y_sum * y_sum));
}
template <typename TItr, typename T>
void Mean(const TItr& x_end, const size_t& n, T* out_mean, const int& stride = 1) {
    auto itr{x_end};
    auto x_sum{*itr};
    for (size_t i = 1; i < n; ++i) {
        --itr;
        x_sum += *itr;
    }
    *out_mean = static_cast<T>(x_sum) / n;
}
template <typename TItr, typename T>
void SqMean(const TItr& x_end, const size_t& n, T* out_mean, const int& stride = 1) {
    // SqMean[x] = E[x*x]
    auto itr{x_end};
    auto x_sum{(*itr) * (*itr)};
    for (size_t i = 1; i < n; ++i) {
        --itr;
        x_sum += (*itr) * (*itr);
    }
    *out_mean = static_cast<T>(x_sum) / n;
}
template <typename TItr1, typename TItr2, typename T>
void ProdMean(const TItr1& x_end, const TItr2& y_end, const size_t& n, T* out_mean, const int& stride = 1) {
    // ProdMean[x, y] = E[x * y]
    auto x_itr{x_end};
    auto y_itr{y_end};
    auto x_sum{(*x_itr) * (*y_itr)};
    for (int i = 1; i < n; ++i) {
        --x_itr;
        --y_itr;
        x_sum += (*x_itr) * (*y_itr);
    }
    *out_mean = static_cast<T>(x_sum) / n;
}
template <typename TItr, typename T>
void Var(const TItr& x_end, const size_t& n, T* out_var, const int& stride = 1) {
    T x_mean{0}, xsq_mean{0};
    Mean(x_end, n, &x_mean, stride);
    SqMean(x_end, n, &xsq_mean, stride);
    *out_var = sqrt((xsq_mean - x_mean * x_mean) * n / (n - 1));
    //*out_var = x_mean;
}

template <typename TItr>
double get_std(const TItr& _begin, const TItr& _end) {
    double xsq{0};
    double x_sum{0};

    for (TItr itr = _begin; itr <= _end; ++itr) {
        auto& tmp_x = *itr;
        x_sum += tmp_x;
        xsq += tmp_x * tmp_x;
    }
    auto len = static_cast<uint32_t>(_end - _begin);
    return sqrt((xsq - x_sum * x_sum / len) / (len - 1));
}
#define EPS 1e-5
template <typename T, size_t length>
void find_low(std::array<T, length> x, T val, int32_t begin, int32_t end, int32_t* low_index) {
    // 要val 要介于x[begin], x[end] 之间
    // 比较 val x, xpr
    //  val <=x && val>xpr, 这是需要寻找的位置
    //  将x整体加上EPS, 这样条件变成了 xpr < val < x
    auto tmp_begin{begin};
    auto tmp_end{end - 1};

    while (tmp_begin < tmp_end) {
        auto i = (tmp_begin + tmp_end) / 2;
        // STD_LOG("[find_low] [{:.5f}, {:.5f}], mid:{:.5f}, val:{:.5f}, begin:{}, end:{}, i:{}", x[tmp_begin],
        // x[tmp_end],
        //        x[i], val, tmp_begin, tmp_end, i);
        if (val < x[i] + EPS) {
            tmp_end = i;
        } else if (val > x[i + 1] + EPS) {
            tmp_begin = i + 1;
        } else {
            *low_index = i;
            return;
        }
    }
}
template <typename T, size_t length>
void find_high(std::array<T, length> x, T val, int32_t begin, int32_t end, int32_t* high_index) {
    // 要求val介于x[begin], x[end-1] 之间
    // 比较 val x, xpr
    //  val <x && val>=xpr, 这是需要寻找的位置
    //  将x整体减去EPS, 这样条件变成了 xpr < val < x
    auto tmp_begin{begin};
    auto tmp_end{end - 1};

    while (tmp_begin < tmp_end) {
        auto i = (tmp_begin + tmp_end) / 2;
        // STD_LOG("[{:.5f}, {:.5f}], mid:{:.5f}, val:{:.5f}, begin:{}, end:{}, i:{}", x[tmp_begin], x[tmp_end], x[i],
        // val,
        //        tmp_begin, tmp_end, i);
        if (val > x[i + 1] - EPS) {
            // val > x[i+1]
            tmp_begin = i + 1;
        } else if (val < x[i] - EPS) {
            // val < x[i]
            tmp_end = i;
        } else {
            *high_index = i;
            return;
        }
    }
}
template <typename T, size_t length>
void find_high_low(const std::array<T, length>& sorted_list, const T& val, int32_t* high_index, int32_t* low_index) {

    // LOG_INFO5("val:{:.5f}", val);
    // for (size_t i = 0; i < sorted_list.size(); ++i) {
    //    LOG_INFO5("v[{}]={:.5f}", i, sorted_list[i]);
    //}
    int32_t end = static_cast<int32_t>(length);
    math_tool::find_low(sorted_list, val, 0, end, low_index);
    // LOG_INFO5("low_index:{}", *low_index);
    int32_t begin = *low_index == 0 ? 0 : *low_index - 1;
    math_tool::find_high(sorted_list, val, begin, end, high_index);
    // LOG_INFO5("high_index:{}", *high_index);
}
}  // namespace math_tool
