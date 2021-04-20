#pragma once
#include "hft/List.hpp"
#include "hft/factor_func.hpp"
#include "hft/math_tool.hpp"
namespace hft {

namespace ff {
template <uint32_t Length>
double Tf1(const List<double, Length> &mid_price_list, const int32_t &timeperiod) {
    // timeperiod = 40
    auto mean_mp = ts_Mean(mid_price_list, timeperiod / 10);
    auto &noise = mid_price_list - mean_mp;
    double var = ff::ts_var(noise, timeperiod);
    var = var > 0.05 ? var : 0.05;
    double amp = (mean_mp.pre(0) - mean_mp.pre(timeperiod)) / var;
    return amp;
}
template <uint32_t Length>
double Tf2(const List<double, Length> &mid_price_list, const int32_t &timeperiod) {
    // timeperiod = 40
    auto mean_mp = ts_Mean(mid_price_list, timeperiod / 10);
    auto &noise = mid_price_list - mean_mp;
    double var = ff::ts_var(noise, timeperiod);
    var = var > 0.05 ? var : 0.05;
    double amp = mean_mp.pre(0) - 2 * mean_mp.pre(timeperiod / 2) + mean_mp.pre(timeperiod);
    return amp / var;
}
template <uint32_t Length>
double Tf3(const List<int32_t, Length> &ask_vol_list, const List<int32_t, Length> &bid_vol_list,
           const int32_t &timeperiod) {
    // timeperiod = 20
    return ff::ts_meand(ask_vol_list, timeperiod) - ff::ts_meand(bid_vol_list, timeperiod);
}
template <uint32_t Length>
double Tf4(const List<int32_t, Length> &ask_vol_list, const int32_t &timeperiod) {
    // timeperiod = 20
    return ff ::ts_trend(ts_Mean(ask_vol_list, 5), timeperiod);
}
template <uint32_t Length>
double Tf5(const List<int32_t, Length> &ask_vol_list, const int32_t &timeperiod) {
    // timeperiod = 20
    return ff::ts_trend2(ts_Mean(ask_vol_list, 5), timeperiod);
}
template <uint32_t Length>
double Tf6(const List<int32_t, Length> &bid_vol_list, const int32_t &timeperiod) {
    // timeperiod = 20
    return ff ::ts_trend(ts_Mean(bid_vol_list, 5), timeperiod);
}
template <uint32_t Length>
double Tf7(const List<int32_t, Length> &bid_vol_list, const int32_t &timeperiod) {
    // timeperiod = 20
    return ff::ts_trend2(ts_Mean(bid_vol_list, 5), timeperiod);
}
};  // namespace ff
};  // namespace hft
