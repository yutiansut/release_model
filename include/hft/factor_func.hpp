#pragma once
#include "hft/List.hpp"
#include "hft/base.h"
#include "hft/math_tool.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
namespace hft {
namespace ff {
template <typename T, uint32_t Len>
List<int8_t, Len> ts_sign(const List<T, Len>& x) {
    List<int8_t, Len> m_sign{x.GetMinLength()};
    for (auto i = x.begin(); i < x.end(); ++i) {
        m_sign.Append(math_tool::sign(*i));
    }
    return m_sign;
}

template <typename T, uint32_t Len>
T ts_max(const List<T, Len>& x, const uint16_t& period) {
    T m_max{x.pre(period - 1)};
    for (auto i = x.end() - period; i < x.end(); ++i) {
        if (*i > m_max) m_max = *i;
    }
    return m_max;
}

template <typename T, uint32_t Len>
T ts_min(const List<T, Len>& x, const uint16_t& period) {
    T m_min{x.pre(period - 1)};
    for (auto i = x.end() - period; i < x.end(); ++i) {
        if (*i < m_min) m_min = *i;
    }
    return m_min;
}

template <typename T, uint32_t Len>
uint16_t ts_argmin(const List<T, Len>& x, const uint16_t& period) {
    T m_min{x.pre(period - 1)};
    uint16_t min_arg{0};
    for (auto i = x.end() - period + 1; i < x.end(); ++i) {
        if (*i < m_min - 1e-7) {
            m_min = *i;
            min_arg = static_cast<uint16_t>(i - x.end()) + period;
        }
    }
    return min_arg;
}

template <typename T, uint32_t Len>
uint16_t ts_argmax(const List<T, Len>& x, const uint16_t& period) {
    T m_max{x.pre(period - 1)};
    uint16_t max_arg{0};
    for (auto i = x.end() - period + 1; i < x.end(); ++i) {
        if (*i > m_max + 1e-7) {
            m_max = *i;
            max_arg = static_cast<uint16_t>(i - x.end()) + period;
        }
    }
    return max_arg;
}

template <typename T, uint32_t Len>
List<T, Len> ts_delta(const List<T, Len>& x, const uint16_t& period) {
    // today’s value of x minus the value of x d days ago
    List<T, Len> tmp{x.GetMinLength()};
    for (auto i = x.begin(); i < x.end() - period; ++i) {
        if (i + period > x.end()) {
            break;
        }
        tmp.Append(*(i + period) - *i);
    }
    return tmp;
}

template <typename T, uint32_t Len>
List<T, Len> ts_delay(const List<T, Len>& x, const uint16_t& period) {
    // today’s value of x minus the value of x d days ago
    List<T, Len> tmp{x.GetMinLength()};
    for (auto i = x.begin(); i < x.end() - period; ++i) {
        tmp.Append(*i);
    }
    return tmp;
}

template <typename T, uint32_t Len>
T ts_sum(const List<T, Len>& x, const uint16_t& period) {
    T tmp_sum{0};
    for (auto i = x.end() - 1; i > x.end() - period - 1; --i) {
        tmp_sum += *i;
    }
    return tmp_sum;
}

template <typename T, uint32_t Len>
const T ts_mean(const List<T, Len>& x, const uint16_t& period) {
    T tmp_sum{0};
    for (auto i = x.end() - 1; i > x.end() - period - 1; --i) {
        tmp_sum += *i;
    }
    return tmp_sum / period;
}

template <typename T, uint32_t Len>
const double ts_meand(const List<T, Len>& x, const uint16_t& period) {
    T tmp_sum{0};
    for (auto i = x.end() - 1; i > x.end() - period - 1; --i) {
        tmp_sum += *i;
    }
    return static_cast<double>(tmp_sum) / period;
}

template <typename T, uint32_t Len>
float ts_meanf(const List<T, Len>& x, const uint16_t& period) {
    T tmp_sum{0};
    for (auto i = x.end() - 1; i > x.end() - period - 1; --i) {
        tmp_sum += *i;
    }
    return static_cast<float>(tmp_sum) / period;
}

template <typename T, uint32_t Len>
int32_t sum_sign(const List<T, Len>& list, const uint16_t& period) {
    auto sign_list = ts_sign(list);
    return ts_sum(sign_list, period);
}

template <typename T, uint32_t Len>
double decay_linear(const List<T, Len>& x, const uint16_t& period) {
    double divisor = 2.0 / (period * (period + 1));
    T tmp{0};
    uint16_t i{period};
    for (auto itr = x.end() - 1; itr > x.end() - 1 - period; --itr, --i) {
        tmp += i * (*itr);
        if (itr <= x.begin()) break;
    }
    return tmp * divisor;
}

template <typename T, uint32_t Len>
float ts_trend(const List<T, Len>& x, const uint16_t& period) {
    double net_amp = x.pre(0) - x.pre(period);
    if (UNLIKELY(fabs(net_amp) < 1e-7)) {
        return 0;
    }
    T tot_amp{0};
    for (auto itr = x.end() - 1; itr > x.end() - period - 1; --itr) {
        tot_amp += fabs(*itr - *(itr - 1));
    }
    // 避免分母出现0
    return net_amp * sqrt(period) / tot_amp;
}

template <typename T, uint32_t Len>
float ts_trend2(const List<T, Len>& x, const uint16_t& period) {
    auto pre_x = x.pre(period);
    if (x.pre(0) > pre_x + 1e-5) {
        return x.pre(0) - ts_min(x, period);
    } else if (x.pre(0) < pre_x - 1e-5) {
        return x.pre(0) - ts_max(x, period);
    } else {
        return 0;
    }
}

template <typename Tx, typename Ty, uint32_t Len>
float ts_corr(const List<Tx, Len>& x, const List<Ty>& y, const uint16_t& period) {
    float out_corr;
    math_tool::Corr(x.end() - 1, y.end() - 1, period, &out_corr);
    return out_corr;
}

template <typename T, uint32_t Len>
float ts_rank(const List<T, Len>& x, const uint16_t& period) {
    // float tmp_rank{0};
    if (UNLIKELY(static_cast<int32_t>(x.end() - x.begin()) < period)) {
        return -1;
    }
    T x0 = x.pre(0);
    std::vector<T> x_copy{x.end() - period, x.end()};
    // sort the vector by value from smaller to bigger
    std::sort(x_copy.begin(), x_copy.end(), std::less<T>());

    uint16_t i_sum{0};
    uint16_t n{0};
    for (uint16_t i = 0; i < period; ++i) {
        if (x0 > x_copy[i] + 1e-5) {
            if (x0 < x_copy[i] + 1e-3) {
                // std::cout << x0 << " > " << x_copy[i] << std::endl;
            }
            continue;
        } else if (x0 < x_copy[i] - 1e-5) {
            if (x0 > x_copy[i] - 1e-3) {
                // std::cout << x0 << " < " << x_copy[i] << std::endl;
            }
            break;
        } else {
            i_sum += i + 1;
            ++n;
        }
    }
    return static_cast<float>(i_sum) / n;
}

template <typename T, uint32_t Len>
float ts_var(const List<T, Len>& x, const uint16_t& period) {
    double tmp{0};
    math_tool::Var(x.end() - 1, period, &tmp);
    return tmp;
}

template <typename T, typename Tw, uint32_t Len, uint32_t Lenw>
float ts_wsum(const List<T, Len>& x, const List<Tw, Lenw>& w, const uint16_t& period) {
    auto tmp_sum{w.pre(0) * x.pre(0)};
    auto weight{w.pre(0)};
    for (uint16_t i = 2; i <= period; ++i) {
        tmp_sum += (*(x.end() - i)) * (*(w.end() - i));
        weight += *(w.end() - i);
    }
    return static_cast<float>(tmp_sum) / weight;
}

template <typename T, typename Tw, uint32_t Len, uint32_t Lenw>
float ts_ewma(const List<T, Len>& x, const List<Tw, Lenw>& w, const uint16_t& period) {
    float tmp_sum{w.pre(0) * x.pre(0)};
    auto weight{w.pre(0)};
    for (uint16_t i = 2; i <= period; ++i) {
        tmp_sum += (*(x.end() - i)) * (*(w.end() - i));
        weight += *(w.end() - i);
    }
    return tmp_sum / weight;
}

template <typename T, uint32_t Len>
float tanh(const List<T, Len>& x, const float alpha = 0.5) {
    auto x0 = x.pre(0);
    float a = alpha * ts_var(x, 120);
    return (1.0 - exp(-x0 / a)) / (1 + exp(-x0 / a));
}

template <typename T, uint32_t Len>
List<double, Len> ts_log(const List<T, Len>& x) {
    List<double, Len> tmp{x.GetMinLength()};
    for (auto v : x) {
        tmp.Append(log(v));
    }
    return tmp;
}

template <typename T, uint32_t Len>
List<double, Len> ts_div(const List<T, Len>& x, const List<T, Len>& y) {
    List<double, Len> tmp{x.GetMinLength()};
    for (size_t i = 0; i < x.cur_size(); ++i) {
        tmp.Append(x[i] / y[i]);
    }
    return tmp;
}
template <typename T, uint32_t Len>
List<double, Len> signLog(const List<T, Len>& x) {
    List<double, Len> tmp{x.GetMinLength()};
    for (auto v : x) {
        tmp.Append(math_tool::sign(v) * log(abs(v) + 1));
    }
    return tmp;
}
template <typename T>
T signLog(const T& val) {
    return math_tool::sign(val) * log(abs(val) + 1);
}

template <typename T, uint32_t Len>
List<double, Len> ts_sqrt(const List<T, Len>& x) {
    List<double, Len> tmp{x.GetMinLength()};
    for (auto v : x) {
        tmp.Append(sqrt(v));
    }
    return tmp;
}

};  // namespace ff
template <class T, uint32_t Length>
List<double, Length> ts_Mean(const List<T, Length>& x, const int32_t& period) {
    List<double, Length> tmp{x.GetMinLength()};
    for (int32_t i = 0; i < period; i++) {
        tmp.Append(0);
    }
    if (static_cast<int32_t>(x.cum_size()) >= period) {
        double tmp_sum = math_tool::get_sum(x.begin(), x.begin() + period - 1);
        tmp.Append(tmp_sum / period);
        for (auto itr = x.begin() + period; itr < x.end(); itr++) {
            tmp_sum += *(itr) - *(itr - period);
            tmp.Append(tmp_sum / period);
        }
    }
    return tmp;
}
template <uint32_t Length>
List<double, Length> ts_Var(const List<double, Length>& x, const int32_t& period) {
    List<double, Length> tmp{x.GetMinLength()};
    for (int32_t i = 0; i < period; i++) {
        tmp.Append(0);
    }
    double tmp_sq_mean{0};
    double tmp_mean{0};
    if (x.cum_size() >= period) {
        math_tool::SqMean(x.begin() + period - 1, period, &tmp_sq_mean);
        math_tool::Mean(x.begin() + period - 1, period, &tmp_mean);
        tmp.Append(tmp_sq_mean - tmp_mean * tmp_mean);
        for (auto itr = x.begin() + period; itr < x.end(); itr++) {
            tmp_sq_mean += (*(itr) * (*itr) - *(itr - period) * (*(itr - period))) / period;
            tmp_mean += (*(itr) - *(itr - period)) / period;
            tmp.Append(tmp_sq_mean - tmp_mean * tmp_mean);
        }
    }
    return tmp;
}
};  // namespace hft
