#pragma once

// 辅助计算因子
// 输入MarketData，输出因#include "hft/EWMCaler.h"
#include "hft/EWMCaler.h"
#include "hft/FastFactorMan.h"
#include "hft/MarketData.h"
#include "hft/TProduct.hpp"
#include "hft/base.h"
#include "hft/factor_func.hpp"
#include "hft/factors.hpp"
#include "hft/math_tool.hpp"
#include "hft/time_series.hpp"
namespace hft {
namespace fast {
class Ff7 {
   public:
    Ff7() {}
    void set_period(const uint32_t period) { m_period = period; }

    uint32_t m_period;
    struct {
        double pre_PIMsp;  // period-1 个周期前
        double min_PIMsp;
        double max_PIMsp;
    } data;
    // 初始化处理
    void init_data(FastFactorMan *factor_man) {
        data.pre_PIMsp = factor_man->PIM_list.pre(m_period - 2);
        data.min_PIMsp = ff::ts_min(factor_man->PIMsp_list, m_period - 1);
        data.max_PIMsp = ff::ts_max(factor_man->PIMsp_list, m_period - 1);
    }
    double eval(FastFactorMan *factor_man) {
        // LOG_INFO3("PIMsp_pr{:.5f} PIMsp:{:.5f} period-2:{:.5f}, period-1:{:.5f}, period:{:.5f}", data.pre_PIMsp,
        //          factor_man->PIMsp, factor_man->PIMsp_list.pre(m_period - 2), factor_man->PIMsp_list.pre(m_period
        //          - 1), factor_man->PIMsp_list.pre(m_period));
        data.min_PIMsp = MIN(data.min_PIMsp, factor_man->PIMsp);
        data.max_PIMsp = MAX(data.max_PIMsp, factor_man->PIMsp);
        // LOG_INFO3("min_PIMsp:{:.5f} max_PIMsp:{:.5f}", data.min_PIMsp, data.max_PIMsp);
        if (factor_man->PIMsp > data.pre_PIMsp + 1e-5) {
            return factor_man->PIMsp - data.min_PIMsp;
        } else if (factor_man->PIMsp < data.pre_PIMsp - 1e-5) {
            return factor_man->PIMsp - data.max_PIMsp;
        } else {
            return 0;
        }
    }
    // 事后处理
    void ExPose(FastFactorMan *factor_man) {
        // 计算极值
        data.pre_PIMsp = factor_man->PIMsp_list.pre(m_period - 1);
        data.min_PIMsp = ff::ts_min(factor_man->PIMsp_list, m_period - 1);
        data.max_PIMsp = ff::ts_max(factor_man->PIMsp_list, m_period - 1);
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
    double eval(FastFactorMan *factor_man) {
        // LOG_INFO3("[Ff8_2] max_PIM:{:.5f} PIM:{:.5f}", MAX(data.max_PIM, factor_man->PIM), factor_man->PIM);
        return MAX(data.max_PIM, factor_man->PIM) * factor_man->PIM;
    }
    void ExPose(FastFactorMan *factor_man) { data.max_PIM = ff::ts_max(factor_man->PIM_list, m_period - 1); }
    // return ff::ts_max(mt, 40) * mt.pre(0);
};

class L2a1 {
   public:
    L2a1() {}
    void init_data(FastFactorMan *factor_man) {}
    double eval(FastFactorMan *factor_man) { return factor_man->ADI - factor_man->ADI_pr; }
    void ExPose(FastFactorMan *factor_man) {}
};

template <uint32_t period = 4>
class L2a2 {
   public:
    L2a2() {}
    void init_data(FastFactorMan *factor_man) {}
    template <size_t length>
    void find_high_low(const std::array<double, length> &sorted_list, const double &val, size_t *high_index,
                       size_t *low_index) {
        // 找到一个元素的位置
        // @sorted_list： 排序好的数组， 排序方式为从小到大， 存在相同的元素
        // 判断浮点数相同的灵敏度为ESP = 1e-5
        const double ESP = 1e-5;
        bool find_high{false}, find_low{false};
        // 初始化
        if (val < sorted_list[0] + ESP) {
            //|| fabs(val - sorted_list[0]) < ESP) {
            find_low = true;
            *low_index = 0;
            //*high_index = 0;
            LOG_INFO3("find low, low_index:{}", *low_index);
        }
        if (val > sorted_list[length - 1] - ESP) {
            //|| fabs(val - sorted_list[length - 1]) < ESP) {
            find_high = 1;
            *high_index = length;
            //*low_index = length;
            // LOG_INFO3("find high_index:{}", *high_index);
        }
        if (find_high && !find_low) *low_index = length;
        if (!find_high && find_low) *high_index = 0;
        for (size_t i = 1; i < length; i++) {
            if (!find_low) {
                if (val > sorted_list[i - 1] && val < sorted_list[i] + ESP) {
                    find_low = true;
                    *low_index = i;
                }
            }
            if (!find_high) {
                if (val > sorted_list[i - 1] - ESP && val < sorted_list[i]) {
                    find_high = true;
                    *high_index = i;
                }
            }
            if (find_high && find_low) {
                break;
            }
        }
    }
    double eval(FastFactorMan *factor_man) {
        auto &adi = factor_man->ADI;
        //        LOG_INFO3("sorted ADI: {:.5f}, {:.5f}, {:.5f}, new ADI:{:.5f}", data.sorted_ADI_list[0],
        //                  data.sorted_ADI_list[1], data.sorted_ADI_list[2], adi);
        size_t high_index, low_index;
        find_high_low(data.sorted_ADI_list, adi, &high_index, &low_index);
        // LOG_INFO3("low_index:{} high index:{}", low_index, high_index);
        return 0.5 * (low_index + high_index) + 1 - 0.5 * period;
    }
    void ExPose(FastFactorMan *factor_man) {
        for (uint32_t i = 0; i < period - 1; ++i) {
            data.sorted_ADI_list[i] = factor_man->ADI_list.pre(i);
        }
        std::sort(data.sorted_ADI_list.begin(), data.sorted_ADI_list.end(), std::less<double>());
    }

   private:
    struct {
        // 从小到大排序
        std::array<double, period - 1> sorted_ADI_list;
    } data;
};  // namespace fast

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
        double weight;
    } data;

   public:
    void init_data(FastFactorMan *factor_man) { ExPose(factor_man); }
    double eval(FastFactorMan *factor_man) {
        double ask_amp = factor_man->ask_amount - data.ask_amount_pr;
        double ask_trend =
            (data.net_ask_amount_amp_pr + ask_amp) / (data.tot_ask_amount_amp_pr + fabs(ask_amp)) * data.weight;
        double bid_amp = factor_man->bid_amount - data.bid_amount_pr;
        double bid_trend =
            (data.net_bid_amount_amp_pr + bid_amp) / (data.tot_bid_amount_amp_pr + fabs(bid_amp)) * data.weight;
        // LOG_INFO3("[ask_trend] {:.5f}", ask_trend);
        // LOG_INFO3("[bid_trend] {:.5f}", bid_trend);
        return ask_trend - bid_trend;
    }
    void ExPose(FastFactorMan *factor_man) {
        data = {0, 0, 0, 0, 0, 0, 0};
        data.weight = sqrt(m_period);
        auto *ask_itr = factor_man->ask_amount_list.itr() + 1 - m_period;
        data.ask_amount_pr = factor_man->ask_amount_list.pre(0);
        data.bid_amount_pr = factor_man->bid_amount_list.pre(0);
        auto ask_amount = *ask_itr;
        for (uint32_t i = 0; i < m_period - 1; ++i) {
            ask_itr++;
            auto amp = *ask_itr - ask_amount;
            data.net_ask_amount_amp_pr += amp;
            data.tot_ask_amount_amp_pr += fabs(amp);
            ask_amount = *ask_itr;
        }
        auto *bid_itr = factor_man->bid_amount_list.itr() + 1 - m_period;
        auto bid_amount = *bid_itr;
        // LOG_INFO3("bid_amount: {:.5f} last:{:.4f}", bid_amount, factor_man->bid_amount_list.pre(m_period - 1));
        for (uint32_t i = 0; i < m_period - 1; ++i) {
            bid_itr++;
            auto amp = *bid_itr - bid_amount;
            data.net_bid_amount_amp_pr += amp;
            data.tot_bid_amount_amp_pr += fabs(amp);
            bid_amount = *bid_itr;
        }
        // LOG_INFO3("bid_amount: {:.5f} last:{:.4f}", bid_amount, factor_man->bid_amount_list.pre(0));
        // LOG_INFO3("net_amp {:.4f}, {:.4f}", data.net_bid_amount_amp_pr,
        //          factor_man->bid_amount_list.pre(0) - factor_man->bid_amount_list.pre(m_period - 1));
    }
};
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
class base_factor {
   public:
    base_factor() {}
    virtual void set_period(const uint32_t &) {}
    virtual void init_data(FastFactorMan *factor_man) {}
    virtual void ExPose(FastFactorMan *factor_man) {}
};

class L2f6 : public base_factor {
   public:
    L2f6() {}
    double eval(FastFactorMan *factor_man) { return math_tool::sign(factor_man->ask_vol - factor_man->bid_vol); }
};
class L2f8 : public base_factor {
   public:
    L2f8() {}
    double eval(FastFactorMan *factor_man) { return log(factor_man->pa / factor_man->pb); }
};
class L2f9 {
   public:
    L2f9() {}

   public:
    struct {
        double ask5_pr2, bid5_pr2;
        std::array<double, 40 + 1> sorted_delta_bid5;
        std::array<double, 40 + 1> sorted_delta_ask5;
    } data;

    void init_data(FastFactorMan *factor_man) { ExPose(factor_man); }
    void ExPose(FastFactorMan *factor_man) {
        data.ask5_pr2 = factor_man->ask5_list.pre(1);
        data.bid5_pr2 = factor_man->bid5_list.pre(1);
        auto tmp = ff::ts_delta(factor_man->bid5_list, 2);
        auto bid_itr = data.sorted_delta_bid5.begin();
        for (auto itr = tmp.end() - 40 + 1; itr < tmp.end(); ++itr) {
            *bid_itr = *itr;
            bid_itr++;
        }
        data.sorted_delta_bid5[39] = 999999;
        data.sorted_delta_bid5[40] = -999999;
        std::sort(data.sorted_delta_bid5.begin(), data.sorted_delta_bid5.end(), std::less<double>());
        tmp = ff::ts_delta(factor_man->ask5_list, 2);
        auto ask_itr = data.sorted_delta_ask5.begin();
        for (auto itr = tmp.end() - 40 + 1; itr < tmp.end(); ++itr) {
            *ask_itr = *itr;
            ask_itr++;
        }
        data.sorted_delta_ask5[39] = 999999;
        data.sorted_delta_ask5[40] = -999999;
        std::sort(data.sorted_delta_ask5.begin(), data.sorted_delta_ask5.end(), std::less<double>());
    }
    double eval(FastFactorMan *factor_man) {
        int32_t high_index, low_index;
        auto tmp_dask5 = factor_man->ask5 - data.ask5_pr2;
        math_tool::find_high_low(data.sorted_delta_ask5, tmp_dask5, &high_index, &low_index);
        double rank_ask = 0.5 * (high_index + low_index) + 1;
        auto tmp_dbid5 = factor_man->bid5 - data.bid5_pr2;
        math_tool::find_high_low(data.sorted_delta_bid5, tmp_dbid5, &high_index, &low_index);
        double rank_bid = 0.5 * (high_index + low_index) + 1;
        // for (size_t i = 0; i < data.sorted_delta_bid5.size(); ++i) {
        //    LOG_INFO5("db[{}] = {}", i, data.sorted_delta_bid5[i]);
        //}
        // LOG_INFO5("[eval] dask5:{:.2f} dbid5:{:.2f}", tmp_dask5, tmp_dbid5);
        // LOG_INFO5("[eval] rank_ask:{:.2f} rank_bid:{:.2f}", rank_ask, rank_bid);
        return (rank_bid + rank_ask - 40) / 40;
    }
};

class rank_1 {
   public:
    rank_1() {}
    struct {
        std::array<double, 99> sorted_ADI_list;
    } data;
    template <size_t length>
    void find_high_low(const std::array<double, length> &sorted_list, const double &val, size_t *high_index,
                       size_t *low_index) {
        // 找到一个元素的位置
        // @sorted_list： 排序好的数组， 排序方式为从小到大， 存在相同的元素
        // 判断浮点数相同的灵敏度为ESP = 1e-5
        const double ESP = 1e-5;
        bool find_high{false}, find_low{false};
        // 初始化
        if (val < sorted_list[0] + ESP) {
            //|| fabs(val - sorted_list[0]) < ESP) {
            find_low = true;
            *low_index = 0;
            //*high_index = 0;
            LOG_INFO3("find low, low_index:{}", *low_index);
        }
        if (val > sorted_list[length - 1] - ESP) {
            //|| fabs(val - sorted_list[length - 1]) < ESP) {
            find_high = 1;
            *high_index = length;
            //*low_index = length;
            // LOG_INFO3("find high_index:{}", *high_index);
        }
        if (find_high && !find_low) *low_index = length;
        if (!find_high && find_low) *high_index = 0;
        for (size_t i = 1; i < length; i++) {
            if (!find_low) {
                if (val > sorted_list[i - 1] && val < sorted_list[i] + ESP) {
                    find_low = true;
                    *low_index = i;
                }
            }
            if (!find_high) {
                if (val > sorted_list[i - 1] - ESP && val < sorted_list[i]) {
                    find_high = true;
                    *high_index = i;
                }
            }
            if (find_high && find_low) {
                break;
            }
        }
    }

    void init_data(FastFactorMan *factor_man) { ExPose(factor_man); }
    void ExPose(FastFactorMan *factor_man) {
        auto ADI_itr = data.sorted_ADI_list.begin();
        auto itr = factor_man->ADI_list.end() - 99;
        for (int i = 0; i < 99; ++i) {
            *ADI_itr = *itr;
            ADI_itr++;
            itr++;
        }
        std::sort(data.sorted_ADI_list.begin(), data.sorted_ADI_list.end(), std::less<double>());
    }
    double eval(FastFactorMan *factor_man) {
        auto adi = factor_man->ADI;
        size_t high_index, low_index;
        find_high_low(data.sorted_ADI_list, adi, &high_index, &low_index);
        return 0.5 * (high_index + low_index) + 1;
    }
};

class rank_2 {
   public:
    rank_2() {}
    struct {
        std::array<float, 101> sorted_ADI_list;
    } data;
    template <typename T, size_t length>
    void find_high_low(const std::array<T, length> &sorted_list, const T &val, int32_t *high_index,
                       int32_t *low_index) {

        // LOG_INFO5("val:{:.5f}", val);
        // for (size_t i = 0; i < sorted_list.size(); ++i) {
        //    LOG_INFO5("v[{}]={:.5f}", i, sorted_list[i]);
        //}
        math_tool::find_low(sorted_list, val, 0, 101, low_index);
        // LOG_INFO5("low_index:{}", *low_index);
        auto begin = *low_index == 0 ? 0 : *low_index - 1;
        math_tool::find_high(sorted_list, val, begin, 101, high_index);
        // LOG_INFO5("high_index:{}", *high_index);
    }

    void init_data(FastFactorMan *factor_man) { ExPose(factor_man); }
    void ExPose(FastFactorMan *factor_man) {
        auto ADI_itr = data.sorted_ADI_list.begin();
        auto itr = factor_man->ADI_list.end() - 99;
        for (int i = 0; i < 99; ++i) {
            *ADI_itr = *itr;
            ADI_itr++;
            itr++;
        }
        data.sorted_ADI_list[99] = 999999;
        data.sorted_ADI_list[100] = -999999;
        std::sort(data.sorted_ADI_list.begin(), data.sorted_ADI_list.end(), std::less<double>());
    }
    double eval(FastFactorMan *factor_man) {
        float adi = factor_man->ADI;
        int32_t high_index{0}, low_index{0};
        find_high_low(data.sorted_ADI_list, adi, &high_index, &low_index);
        return 0.5 * (high_index + low_index) + 1;
    }
};
class L2p1 {
   public:
    L2p1() {}

   public:
    struct {
        double pa_pr;
        double pb_pr;
    } data;
    void init_data(FastFactorMan *factor_man) { ExPose(factor_man); }
    void ExPose(FastFactorMan *factor_man) {
        data.pa_pr = factor_man->pa;
        data.pb_pr = factor_man->pb;
    }
    double eval(FastFactorMan *factor_man) { return factor_man->pa - factor_man->pb - data.pa_pr + data.pb_pr; }
};
class L2p2 {
   public:
    L2p2() {}

   public:
    struct {
        std::array<double, 7> sorted_pa_list;
        std::array<double, 7> sorted_pb_list;
    } data;
    void init_data(FastFactorMan *factor_man) { ExPose(factor_man); }
    void ExPose(FastFactorMan *factor_man) {
        auto pa_itr = data.sorted_pa_list.begin();
        for (auto itr = factor_man->pa_list.end() - 5; itr < factor_man->pa_list.end(); itr++) {
            *pa_itr = *itr;
            pa_itr++;
        }
        data.sorted_pa_list[5] = -999999;
        data.sorted_pa_list[6] = 999999;
        std::sort(data.sorted_pa_list.begin(), data.sorted_pa_list.end(), std::less<double>());

        auto pb_itr = data.sorted_pb_list.begin();
        for (auto itr = factor_man->pb_list.end() - 5; itr < factor_man->pb_list.end(); itr++) {
            *pb_itr = *itr;
            pb_itr++;
        }
        data.sorted_pb_list[5] = -999999;
        data.sorted_pb_list[6] = 999999;
        std::sort(data.sorted_pb_list.begin(), data.sorted_pb_list.end(), std::less<double>());
    }
    double eval(FastFactorMan *factor_man) {
        int32_t high{0}, low{0};
        math_tool::find_high_low(data.sorted_pa_list, factor_man->pa, &high, &low);
        auto rank_pa = 0.5 * (high + low) + 1;
        math_tool::find_high_low(data.sorted_pb_list, factor_man->pb, &high, &low);
        auto rank_pb = 0.5 * (high + low) + 1;
        // LOG_INFO5("rank_pa:{:.2f} rank_pb:{:.2f}", rank_pa, rank_pb);
        return rank_pa - rank_pb;
    }
};
class VOI {
   public:
    VOI() {}

   public:
    struct {
        double ask1_pr;
        double bid1_pr;
        int32_t bidv1_pr;
        int32_t askv1_pr;
    } data;
    void init_data(FastFactorMan *factor_man) { ExPose(factor_man); }
    void ExPose(FastFactorMan *factor_man) {
        data.ask1_pr = factor_man->ask1;
        data.bid1_pr = factor_man->bid1;
        data.askv1_pr = factor_man->askv1;
        data.bidv1_pr = factor_man->bidv1;
    }
    double eval(FastFactorMan *factor_man) {
        int32_t delta_bidv1{0}, delta_askv1{0};
        auto &ask1 = factor_man->ask1;
        if (Compare(ask1, data.ask1_pr) == 0) {
            delta_askv1 = factor_man->askv1 - data.askv1_pr;
        } else if (Compare(ask1, data.ask1_pr) == -1) {
            delta_askv1 = factor_man->askv1;
        }
        auto &bid1 = factor_man->bid1;
        if (Compare(bid1, data.bid1_pr) == 1) {
            delta_bidv1 = factor_man->bidv1;
        } else if (Compare(bid1, data.bid1_pr) == 0) {
            delta_bidv1 = factor_man->bidv1 - data.bidv1_pr;
        }
        return delta_bidv1 - delta_askv1;
    }
};

class a1f10 : public base_factor {
   public:
    a1f10() {}
    struct {
        double MPBVS;
    } data;
    void init_data(FastFactorMan *factor_man) { ExPose(factor_man); }
    double eval(FastFactorMan *factor_man) { return factor_man->m_MPBVS.GetMean() - data.MPBVS; }
    void ExPose(FastFactorMan *factor_man) { data.MPBVS = factor_man->m_MPBVS.GetMean(); }
};

class a1f11 {
   public:
    a1f11() {}
    struct {
        double mid_price;
        double MPB_pr[3];
    } data;
    void init_data(FastFactorMan *factor_man) {
        data.mid_price = factor_man->mid_price;
        auto &wvap_list = factor_man->wvap_list;
        auto &mp_list = factor_man->mid_price_list;
        data.MPB_pr[0] = wvap_list.pre(2) - 0.5 * (mp_list.pre(2) + mp_list.pre(3));
        data.MPB_pr[1] = wvap_list.pre(1) - 0.5 * (mp_list.pre(1) + mp_list.pre(2));
        data.MPB_pr[2] = wvap_list.pre(0) - 0.5 * (mp_list.pre(0) + mp_list.pre(1));
    }
    void ExPose(FastFactorMan *factor_man) {
        auto MPB_pr = factor_man->wvap - 0.5 * (factor_man->mid_price + data.mid_price);
        data.MPB_pr[0] = data.MPB_pr[1];
        data.MPB_pr[1] = data.MPB_pr[2];
        data.MPB_pr[2] = MPB_pr;
        data.mid_price = factor_man->mid_price;
    }
    double eval(FastFactorMan *factor_man) {
        auto MPB = factor_man->wvap - 0.5 * (factor_man->mid_price + data.mid_price);
        return MPB - data.MPB_pr[0];
    }
};

class a1f12 {
   public:
    a1f12() {}
    struct {
        double mid_price;
        double MPB_pr[4];
    } data;
    void init_data(FastFactorMan *factor_man) {
        data.mid_price = factor_man->mid_price;
        auto &wvap_list = factor_man->wvap_list;
        auto &mp_list = factor_man->mid_price_list;
        data.MPB_pr[0] = wvap_list.pre(3) - 0.5 * (mp_list.pre(3) + mp_list.pre(4));
        data.MPB_pr[1] = wvap_list.pre(2) - 0.5 * (mp_list.pre(2) + mp_list.pre(3));
        data.MPB_pr[2] = wvap_list.pre(1) - 0.5 * (mp_list.pre(1) + mp_list.pre(2));
        data.MPB_pr[3] = wvap_list.pre(0) - 0.5 * (mp_list.pre(0) + mp_list.pre(1));
    }
    void ExPose(FastFactorMan *factor_man) {
        auto MPB_pr = factor_man->wvap - 0.5 * (factor_man->mid_price + data.mid_price);
        data.MPB_pr[0] = data.MPB_pr[1];
        data.MPB_pr[1] = data.MPB_pr[2];
        data.MPB_pr[2] = data.MPB_pr[3];
        data.MPB_pr[3] = MPB_pr;
        data.mid_price = factor_man->mid_price;
    }
    double eval(FastFactorMan *factor_man) {
        auto MPB = factor_man->wvap - 0.5 * (factor_man->mid_price + data.mid_price);
        return MPB - data.MPB_pr[0];
    }
};

class a1f13 {
   public:
    a1f13() {}
    struct {
        double VOI_pr;
    } data;
    void init(FastFactorMan *factor_man) { ExPose(factor_man); }
    void ExPose(FastFactorMan *factor_man) { data.VOI_pr = factor_man->VOI_list.pre(2); }
    double eval(FastFactorMan *factor_man) { return factor_man->GetVOI() - data.VOI_pr; }
};

template <int32_t period>
class DeltaVPI {
   public:
    DeltaVPI() {}
    struct {
        double VPI;
    } data;
    void init_data(FastFactorMan *factor_man) { ExPose(factor_man); }
    void ExPose(FastFactorMan *factor_man) { data.VPI = factor_man->VPI_list.pre(period - 1); }
    double eval(FastFactorMan *factor_man) { return factor_man->VPI - data.VPI; }
};

using a1f14 = DeltaVPI<3>;
using a1f15 = DeltaVPI<4>;
// a2f1 = factor::get_a2(ADI_list, 2);
// a1f4 = factor::get_a1(DMB_list, 1);
// a1f6 = factor::get_a1(EVOI_list, 4);
// a1f7 = factor::get_a1(GTVol_list, 3);
// a1f9 = factor::get_a1(L2f1_list, 4);
}  // namespace fast
}  // namespace hft
