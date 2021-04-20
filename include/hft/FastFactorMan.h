#pragma once

#if defined(CAL_L2a1) || defined(CAL_L2a2) || defined(CAL_L2a3) || defined(CAL_L2a4) || defined(CAL_a1f1) ||           \
    defined(CAL_a2f1) || defined(CAL_a2f3)
#define CAL_ADI
#endif
#if defined(CAL_a1f2) || defined(CAL_a2f4) || defined(CAL_a2f5) || defined(CAL_a2f6) || defined(CAL_a2f7)
#define CAL_AsyBidAsk
#endif

#if defined(CAL_a1f3) || defined(CAL_a2f23) || defined(CAL_a2f24) || defined(CAL_a2f25)
#define CAL_SumSizeDiff
#endif

#if defined(CAL_a1f4) || defined(CAL_a2f8)
#define CAL_DMB
#endif

#if defined(CAL_a1f5) || defined(CAL_a1f6) || defined(CAL_a2f9) || defined(CAL_a2f10)
#define CAL_EVOI
#endif

#if defined(CAL_a1f7) || defined(CAL_a1f8) || defined(CAL_a2f11) || defined(CAL_a2f12)
#define CAL_GTVol
#endif

#if defined(CAL_a1f9) || defined(CAL_a2f13) || defined(CAL_a2f14)
#define CAL_L2f1
#endif

#if defined(CAL_a1f10) || defined(CAL_a2f15) || defined(CAL_a2f16) || defined(CAL_a2f16)
#define CAL_MPBVS
#endif

#if defined(CAL_a1f11) || defined(CAL_a1f12) || defined(CAL_a2f17) || defined(CAL_a2f18) || defined(CAL_a2f19)
#define CAL_MPB
#endif

#if defined(CAL_a1f13) || defined(CAL_a2f27) || defined(CAL_a2f28)
#define CAL_VOI
#endif

#if defined(CAL_a1f14) || defined(CAL_a1f15) || defined(CAL_a2f2) || defined(CAL_a2f29) || defined(CAL_a2f30)
#define CAL_VPI
#endif

#if defined(CAL_a2f21) || defined(CAL_a2f21) || defined(CAL_a2f22) || defined(CAL_a2f23)
#define CAL_SignVol_1
#endif

// 辅助计算因子
// 输入MarketData，输出因#include "hft/EWMCaler.h"
#include "hft/EWMCaler.h"
#include "hft/MarketData.h"
#include "hft/TProduct.hpp"
#include "hft/base.h"
#include "hft/factor_func.hpp"
#include "hft/factors.hpp"
#include "hft/math_tool.hpp"
#include "hft/time_series.hpp"
namespace hft {
namespace fast {
class FastFactorMan : public TProduct {
   public:
    FastFactorMan(const std::string &name = "", const uint32_t &min_length = 125) : TProduct{name} {
        LOG_INFO1("[FastFactorMan] name:{} min_length:{}", name, min_length);
    }
    void AppendTick(const md::l2 &tick);
    void init_data(md::l2 *tick_col, const uint32_t &length) {
        // wvap = tp_list.pre(0);
        // mid_price_pr = mp.pre(0);
        // wvap = (*turn_over_itr) / trade_vol / multiplier;
    }
    void eval(const md::l2 &tick) {
        turn_over = tick.lastamount - amount_pr;
        trade_vol = tick.lastvolume - volume_pr;
        wvap = turn_over / trade_vol / multiplier;
        PIM = (wvap - 0.5 * (mid_price + mid_price_pr));
        ADI = static_cast<double>(sum_bidv - sum_askv) / (sum_bidv + sum_askv);
    }
    void ExPose(const md::l2 &tick) {
        amount_pr = tick.lastamount;
        volume_pr = tick.lastvolume;
        mid_price_pr = mid_price;
        PIM_list.Append(PIM);
        ADI_pr = ADI;
    }
    void ReSet() {
        wvap_list.ReSet();
        PIM_list.ReSet();
        ADI_list.ReSet();
        ask_amount_list.ReSet();
        bid_amount_list.ReSet();
    }

   private:
    uint32_t m_min_length;

   public:
    double amount_pr, turn_over;

    int32_t volume_pr, trade_vol;
    double spread;
    double wvap;

    double mid_price, mid_price_pr;
    double PIM;  // wvap - 0.5*(mp[-1] + mp[0])

    int32_t sum_bidv, sum_askv;
    double ADI, ADI_pr;  //

    double ask_amount, bid_amount;

   public:
    List<double> wvap_list;
    List<double> PIM_list;
    List<double> ADI_list;
    List<double> ask_amount_list;
    List<double> bid_amount_list;
};
class Ff7 {
   public:
    Ff7() {}
    void set_period(const uint32_t period) { m_period = period; }

    uint32_t m_period;
    struct {
        double pre_PIM;  // period-1 个周期前
        double min_PIM;
        double max_PIM;
    } data;
    // 初始化处理
    void init_data(FastFactorMan *factor_man) {
        data.pre_PIM = factor_man->PIM_list.pre(m_period - 2);
        data.min_PIM = ff::ts_min(factor_man->PIM_list, m_period - 1);
        data.max_PIM = ff::ts_max(factor_man->PIM_list, m_period - 1);
    }
    double eval(FastFactorMan *factor_man) {
        if (factor_man->PIM > data.pre_PIM + 1e-5) {
            return (factor_man->PIM - data.min_PIM) / factor_man->spread;
        } else if (factor_man->PIM < data.pre_PIM - 1e-5) {
            return (factor_man->PIM - data.max_PIM) / factor_man->spread;
        } else {
            return 0;
        }
    }
    // 事后处理
    void ExPose(FastFactorMan *factor_man) {

        // 计算极值
        data.pre_PIM = factor_man->PIM_list.pre(m_period - 2);
        data.min_PIM = MIN(data.min_PIM, factor_man->PIM);
        data.max_PIM = MAX(data.max_PIM, factor_man->PIM);
    }
};
class Ff8_2 {
   public:
    Ff8_2() : m_period{40} {}
    void set_period(int period) { m_period = period; }

   public:
    uint32_t m_period;
    struct {
        double max_PIM;
    } data;
    void init_data(FastFactorMan *factor_man) { data.max_PIM = ff::ts_max(factor_man->PIM_list, m_period - 1); }
    double eval(FastFactorMan *factor_man) { return MAX(data.max_PIM, factor_man->PIM) * factor_man->PIM; }
    void ExPose(FastFactorMan *factor_man) { data.max_PIM = MAX(data.max_PIM, factor_man->PIM); }
    // return ff::ts_max(mt, 40) * mt.pre(0);
};

class L2a1 {
   public:
    L2a1() {}
    double eval(FastFactorMan *factor_man) { return factor_man->ADI - factor_man->ADI_pr; }
    void ExPose(FastFactorMan *factor_man) {}
};

template <uint32_t period = 4>
class L2a2 {
   public:
    L2a2() {}
    void init_data(FastFactorMan *factor_man) {}
    double eval(FastFactorMan *factor_man) {
        uint32_t i_sum{0};
        uint32_t n{0};
        auto &adi = factor_man->ADI;
        for (uint32_t i = 0; i < period; ++i) {
            auto &tmp_adi = data.sorted_ADI_list[i];
            if (adi > tmp_adi + 1e-5) {
                continue;
            } else if (adi < tmp_adi - 1e-5) {
                break;
            } else {
                i_sum += i + 1;
                ++n;
            }
        }
        return n != 0 ? static_cast<double>(i_sum) / n - 0.5 * period : 0.5 * period;
    }
    void ExPose(FastFactorMan *factor_man) {
        for (int i = 0; i < period - 1; ++i) {
            data.sorted_ADI_list[i] = factor_man->ADI_list.pre(i);
        }
        std::sort(data.sorted_ADI_list.begin(), data.sorted_ADI_list.end(), std::less<double>());
    }

   private:
    struct {
        // 从小到大排序
        std::array<double, period - 1> sorted_ADI_list;
    } data;
};

class L2f10 {
   public:
    L2f10() {}
    void set_period(const uint32_t &period) { m_period = period; }

   private:
    uint32_t m_period;
    struct {
        double ask_amount_pr;
        double bid_amount_pr;
        double net_ask_amount_amp_pr;
        double tot_ask_amount_amp_pr;
        double net_bid_amount_amp_pr;
        double tot_bid_amount_amp_pr;
    } data;

   public:
    void init_data(FastFactorMan *factor_man) {
        data = {0, 0, 0, 0, 0, 0};
        auto *ask_itr = factor_man->ask_amount_list.itr() + 2 - m_period;
        auto ask_amount = *ask_itr;
        for (uint32_t i = 1; i < m_period - 1; ++i) {
            ask_itr++;
            auto amp = *ask_itr - ask_amount;
            data.net_ask_amount_amp_pr += amp;
            data.tot_ask_amount_amp_pr += fabs(amp);
            ask_amount = *ask_itr;
        }
        auto *bid_itr = factor_man->bid_amount_list.itr() + 2 - m_period;
        auto bid_amount = *bid_itr;
        for (uint32_t i = 1; i < m_period - 1; ++i) {
            bid_itr++;
            auto amp = *bid_itr - bid_amount;
            data.net_bid_amount_amp_pr += amp;
            data.tot_bid_amount_amp_pr += fabs(amp);
            bid_amount = *bid_itr;
        }
    }
    double eval(FastFactorMan *factor_man) {
        double ask_amp = factor_man->ask_amount - data.ask_amount_pr;
        double ask_trend = (data.net_ask_amount_amp_pr + ask_amp) / (data.tot_ask_amount_amp_pr + fabs(ask_amp));
        double bid_amp = factor_man->bid_amount - data.bid_amount_pr;
        double bid_trend = (data.net_bid_amount_amp_pr + bid_amp) / (data.tot_bid_amount_amp_pr + fabs(bid_amp));
        return ask_trend + bid_trend;
    }
    void ExPose(FastFactorMan *factor_man) {}
};
//    return ff::ts_trend(ask_amount, period) - ff::ts_trend(bid_amount, period);
}  // namespace fast
}  // namespace hft
