#pragma once
#include "hft/factor_func.hpp"
#include <cmath>
#include <unordered_map>
namespace hft {
namespace factor {
enum class Factor {
    Ff1,
    Length,  // it's not a factor, only a counter
};

template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
float Ff1(const List<T1, Len1> &bid1, const List<T2, Len2> &trade_vol) {
    auto delta_price = ff::ts_delta(ff::ts_log(bid1), 1);
    delta_price *= 10000;
    return ff::ts_wsum(delta_price, ff::ts_sqrt(trade_vol) + 0.01, 10);
}

template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
float Ff2(const List<T1, Len1> &bid1, const List<T2, Len2> &bid_vol) {
    auto delta_price = ff::ts_delta(ff::ts_log(bid1), 1);
    delta_price *= 10000;
    return ff::ts_wsum(delta_price, ff::ts_sqrt(bid_vol + 1) + 0.01, 20);
}

template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
float Ff3(const List<T1, Len1> &bid1, const List<T2, Len2> &bid_vol) {
    auto delta_price = ff::ts_delta(ff::ts_log(bid1), 1);
    delta_price *= 10000;
    return ff::ts_wsum(delta_price, bid_vol + 1, 10);
}
template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
double Ff4(const List<T1, Len1> &mp, const List<T2, Len2> &sp, const float &preW, const float &alpha = 0.70f) {
    // alpha = 0.70
    // x = (tk.mp - tk.mp.shift(1)) / tk.sp.shift(1)
    auto tmp = (mp.pre(0) - mp.pre(1)) / sp.pre(1);
    return alpha * tmp + (1 - alpha) * preW;
}

template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
double Ff5(const List<T1, Len1> &mp, const List<T2, Len2> &last_price) {
    auto tmp = (1 - mp.pre(0) / last_price.pre(0)) * 1e5;
    return tmp;
}

template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
double Ff6(const List<T1, Len1> &TP, const List<T2, Len2> &mp, const List<T1, Len1> &sp) {
    return (TP.pre(0) - 0.5 * (mp.pre(0) + mp.pre(1))) / sp.pre(0);
}

template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
double Ff7_10(const List<T1, Len1> &TP, const List<T2, Len2> &mp, const List<T1, Len1> &sp) {
    if (TP.cum_size() < 10) {
        return 0;
    } else {
        auto mt = TP.copy();
        for (int i = 1; i < static_cast<int>(mt.end() - mt.begin()); i++) {
            mt[i] = (mt[i] - 0.5 * (mp[i - 1] + mp[i])) / sp[i];
        }
        LOG_INFO3("[Ff7_10] PIM:{:.5f}", mt.pre(0));
        return ff::ts_trend2(mt, 10);
    }
}

template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
double Ff7_20(const List<T1, Len1> &TP, const List<T2, Len2> &mp, const List<T1, Len1> &sp) {
    if (TP.cum_size() < 20) {
        return 0;
    } else {
        auto mt = TP.copy();
        for (int i = 1; i < static_cast<int>(mt.end() - mt.begin()); i++) {
            mt[i] = (mt[i] - 0.5 * (mp[i - 1] + mp[i])) / sp[i];
        }
        return ff::ts_trend2(mt, 20);
    }
}

template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
double Ff7_40(const List<T1, Len1> &TP, const List<T2, Len2> &mp, const List<T1, Len1> &sp) {
    if (TP.cum_size() < 40) {
        return 0;
    } else {
        auto mt = TP.copy();
        for (int i = 1; i < static_cast<int>(mt.end() - mt.begin()); i++) {
            mt[i] = (mt[i] - 0.5 * (mp[i - 1] + mp[i])) / sp[i];
        }
        return ff::ts_trend2(mt, 40);
    }
}

template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
double Ff7_80(const List<T1, Len1> &TP, const List<T2, Len2> &mp, const List<T1, Len1> &sp) {
    auto mt = TP.copy();
    mt -= 0.5 * ff::ts_delay(mp, 1);
    mt -= 0.5 * mp;
    mt /= sp;
    return ff::ts_trend2(mt, 80);
}

template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
double Ff7(const List<T1, Len1> &TP, const List<T2, Len2> &mp, const List<T1, Len1> &sp) {
    if (TP.cum_size() < 80) {
        return 0;
    } else {
        auto mt = TP.copy();
        mt -= 0.5 * ff::ts_delay(mp, 1);
        mt -= 0.5 * mp;
        mt /= sp;
        auto tmp = 0.2 * ff::ts_trend2(mt, 10);
        tmp += 0.2 * ff::ts_trend2(mt, 20);
        tmp += 0.1 * ff::ts_trend2(mt, 40);
        tmp += 0.1 * ff::ts_trend2(mt, 80);
        return tmp;
    }
}
template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
double Ff8_1(const List<T1, Len1> &TP, const List<T2, Len2> &mp, const List<T1, Len1> &sp) {
    auto mt = TP.copy();
    for (int i = 1; i < static_cast<int>(mt.end() - mt.begin()); i++) {
        mt[i] = mt[i] - 0.5 * (mp[i - 1] + mp[i]);
    }
    return -ff::ts_min(mt, 40) * mt.pre(0);
}

template <typename T1, uint32_t Len1, typename T2, uint32_t Len2>
double Ff8_2(const List<T1, Len1> &TP, const List<T2, Len2> &mp, const List<T1, Len1> &sp) {
    auto mt = TP.copy();
    int max_size = static_cast<int>(mt.end() - mt.begin());
    for (int i = MAX(max_size - 42, 0); i < max_size; i++) {
        mt[i] = mt[i] - 0.5 * (mp[i - 1] + mp[i]);
    }
    // LOG_INFO3("[Ff8_2] mt:{:.5f}, max_mt:{:.5f}, max_size:{}", mt.pre(0), ff::ts_max(mt, 40),
    //          static_cast<int>(mt.end() - mt.begin()));
    return ff::ts_max(mt, 40) * mt.pre(0);
}

template <typename T, uint32_t Len1, uint32_t Len2>
float VPI(const List<T, Len1> &ask_vol, const List<T, Len2> &bid_vol) {
    for (int i = 0; i < ask_vol.GetMinLength(); ++i) {
        if (bid_vol.pre(i) + ask_vol.pre(i) != 0) {
            return (ask_vol.pre(i) - bid_vol.pre(i)) / (bid_vol.pre(i) + ask_vol.pre(i));
        }
    }
    return 0;
}
template <typename T, uint32_t Len1, uint32_t Len2>
float GTVol(const List<T, Len1> &ask_vol, const List<T, Len2> &bid_vol) {
    for (int i = 0; i < ask_vol.GetMinLength(); ++i) {
        if (bid_vol.pre(i) + ask_vol.pre(i) != 0) {
            return (ask_vol.pre(i) - bid_vol.pre(i));
        }
    }
    return 0;
}
template <typename T, uint32_t Len>
float L2f1(List<double, Len> ask[5], List<int32_t, Len> askv[5], List<double, Len> bid[5], List<int32_t, Len> bidv[5],
           const List<T, Len> &bid_amount, const List<T, Len> &ask_amount) {
    float tmp{0};
    for (int i = 0; i < 5; ++i) {
        tmp -= ask[i].pre(0) * askv[i].pre(0) - ask[i].pre(1) * askv[i].pre(1);
        tmp += bid[i].pre(0) * bidv[i].pre(0) - bid[i].pre(1) * bidv[i].pre(1);
    }
    tmp += ask_amount.pre(0);
    tmp -= bid_amount.pre(0);
    return tmp * 1e-5;
}
template <typename T, uint32_t Len>
int8_t L2f6(const List<T, Len> &ask_vol, const List<T, Len> &bid_vol) {
    return math_tool::sign(ask_vol.pre(0) - bid_vol.pre(0));
}
template <typename T, uint32_t Len>
float L2f7(List<T, Len> askv[5], List<T, Len> bidv[5]) {
    auto bidV = bidv[0].pre(0) + bidv[1].pre(0) + bidv[2].pre(0) + bidv[3].pre(0) + bidv[4].pre(0);
    auto askV = askv[0].pre(0) + askv[1].pre(0) + askv[2].pre(0) + askv[3].pre(0) + askv[4].pre(0);
    return log(bidV) - log(askV);
}
template <typename T, uint32_t Len, typename T2>
float L2f8(const List<T, Len> &ask5, const List<T, Len> &bid5, const List<T2, Len> &mp) {
    return log(ask5.pre(0) - mp.pre(0)) - log(mp.pre(0) - bid5.pre(0));
}

template <typename T, uint32_t Len>
float L2f9(const List<T, Len> &ask5, const List<T, Len> &bid5) {
    auto tk_bid = ff::ts_rank(ff::ts_delta(bid5, 2), 40);
    auto tk_ask = ff::ts_rank(ff::ts_delta(ask5, 2), 40);
    // LOG_INFO5("[L2f9] rank_ask:{:.2f} rank_bid:{:.2f}", tk_ask, tk_bid);
    return (tk_bid + tk_ask - 40) / 40;
}

template <typename T, uint32_t Len>
double L2f10(const List<T, Len> &ask_amount, const List<T, Len> &bid_amount, const uint16_t &period) {
    // LOG_INFO3("old[ask_amount_trend] period:{} {:.5f}", period, ff::ts_trend(ask_amount, period));
    // LOG_INFO3("old[bid_amount_trend] period:{} {:.5f}", period, ff::ts_trend(bid_amount, period));
    return ff::ts_trend(ask_amount, period) - ff::ts_trend(bid_amount, period);
}

template <typename T, uint32_t Len>
float L2f11(const List<T, Len> &ask_vol, const List<T, Len> &bid_vol, const uint16_t &period = 20) {
    return (ff::ts_rank(ask_vol, period) - ff::ts_rank(bid_vol, period)) / period;
}

template <typename T, uint32_t Len>
float L2f12(const List<T, Len> &ask_amount, const List<T, Len> &bid_amount, const uint16_t &period = 20) {
    return (ff::ts_rank(ask_amount, period) - ff::ts_rank(bid_amount, period)) / period;
}

template <typename T, uint32_t Len>
double L2f13(const List<T, Len> &ask_vol_list, const List<T, Len> &bid_vol_list) {
    auto A = ff::decay_linear(ff::ts_delta(ff::signLog(ask_vol_list), 1), 40);
    auto B = ff::decay_linear(ff::ts_delta(ff::signLog(bid_vol_list), 1), 40);
    return A - B;
}

template <typename T, uint32_t Len>
double L2f14(const List<T, Len> &ask_vol_list, const List<T, Len> &bid_vol_list) {
    // return ff::ts_rank(bid_vol_list, 40);
    auto A = log(ff::ts_rank(ask_vol_list, 40) + 1);
    auto B = log(ff::ts_rank(bid_vol_list, 40) + 1);
    return A - B;
}

template <typename T, uint32_t Len>
float L2a1(const List<T, Len> &ADI_list) {
    return ff::ts_delta(ADI_list, 1).pre(0);
}

template <typename T, uint32_t Len>
float L2a2(const List<T, Len> &ADI_list) {
    return ff::ts_rank(ADI_list, 4) - 2;
}

template <typename T, uint32_t Len>
float L2a3(const List<T, Len> &ADI_rank_5) {
    return ff::ts_rank(ADI_rank_5, 120) - 60;
}

template <typename T, uint32_t Len>
float L2a4(const List<T, Len> &ADI_rank_6_rank_80) {
    return ff::ts_rank(ADI_rank_6_rank_80, 120) - 60;
}

template <typename T, uint32_t Len>
float L2p1(const List<T, Len> &ask5, const List<T, Len> &bid5, const List<T, Len> &mp) {
    auto pa = ask5.copy();
    pa -= mp;
    auto pb = mp.copy();
    pb -= bid5;
    return ff::ts_delta(pa, 1).pre(0) - ff::ts_delta(pb, 1).pre(0);
}
template <typename T, uint32_t Len>
float L2p2(const List<T, Len> &ask5, const List<T, Len> &bid5, const List<T, Len> &mp) {
    auto pa = ask5.copy();
    pa -= mp;
    auto pb = mp.copy();
    pb -= bid5;
    return ff::ts_rank(pa, 6) - ff::ts_rank(pb, 6);
}

template <typename T, uint32_t Len>
float get_a1(const List<T, Len> &x, const uint16_t &period) {
    return -x.pre(period) + x.pre(0);
}

template <typename T, uint32_t Len>
float get_a2(const List<T, Len> &x, const uint16_t &period) {
    return x.pre(0) - 2 * x.pre(period) + x.pre(2 * period);
}

};  // namespace factor
};  // namespace hft
