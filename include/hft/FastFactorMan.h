#pragma once

#include "hft/Define_factor.hpp"

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
    FastFactorMan(const std::string &name = "", const uint32_t &min_length = 125);
    void init_data(md::l2 *tick_col, const uint32_t &length);
    void eval(const md::l2 &tick);
    void ExPose(const md::l2 &tick);
    void ReSet();

   private:
    void AppendTick(const md::l2 &tick);
    void process_first_tick(const md::l2 &tick);
    void guess_trade_price(const md::l2 &tick);

   public:
    inline double GetADI();
    inline double GetDMB();
    inline double GetMPB();
    inline double GetMPBVS();
    inline double GetVOI();
    inline double GetVPI();
    inline double GetEVOI();
    inline double GetGTVol();
    inline double GetL2f1(const md::l2 &tick);
    inline double GetAsyBidAsk(const md::l2 &tick);

   public:
    // 派生因子
    inline double a1f4();
    inline double a1f5();
    inline double a1f6();
    inline double a1f7();
    inline double a1f8();
    inline double a1f9();
    inline double a1f10();
    inline double a1f11();
    inline double a1f12();
    inline double a1f13();
    inline double a1f14();
    inline double a1f15();
    inline double a2f5();
    inline double a2f6();
    inline double a2f7();
    inline double a2f12();
    inline double a2f13();

    // a1f4 = factor::get_a1(DMB_list, 1);

   private:
    uint32_t m_min_length;

   public:
    double amount_pr, turn_over;

    int32_t volume_pr, trade_vol;
    int32_t ask_vol, bid_vol;
    int32_t take_ask[5], take_bid[5];
    int32_t askv_pr[5], bidv_pr[5];
    double bid_pr[5], ask_pr[5];
    double ask_amount, bid_amount;
    double pa, pb;
    double last_price;
    double spread;
    double wvap;

    double mid_price, mid_price_pr;
    double PIM;    // wvap - 0.5*(mp[-1] + mp[0])
    double PIMsp;  // wvap - 0.5*(mp[-1] + mp[0])
    double VPI;
    double DMB;
#ifdef USE_MPB
    double MPB;
#endif
#ifdef USE_L2f1
    double L2f1;
#endif
#ifdef USE_GTVol
    double GTVol;
#endif
#ifdef USE_MPBVS
    double MPBVS;
#endif
#ifdef USE_AsyBidAsk
    double AsyBidAsk;
#endif
    int32_t sum_bidv, sum_askv;
    double ADI, ADI_pr;  //
    double ask1, bid1;
    double ask1_pr, bid1_pr;
    int32_t askv1, bidv1;
    int32_t askv1_pr, bidv1_pr;
    double ask5, bid5;

   public:
#ifdef USE_VOI
    double VOI;
#endif
#ifdef USE_EVOI
    double EVOI;
#endif

   public:
    EWMCaler<double> m_MPBVS;
    EWMCaler<double> m_EVOI;
    // LOG_INFO("[{}_MPBVS] {}", name, m_MPBVS.GetMean());

   public:
    List<double> wvap_list;
    List<double> mid_price_list;
    List<double> PIM_list;
    List<double> PIMsp_list;
    List<double> ADI_list;
    List<double> ask_amount_list;
    List<double> bid_amount_list;
    List<double> ask5_list;
    List<double> bid5_list;
    List<double> pa_list;
    List<double> pb_list;
#ifdef USE_MPBVS
    List<double> MPBVS_list;
#endif
#ifdef USE_VPI
    List<double> VPI_list;
#endif
#ifdef USE_VOI
    List<double> VOI_list;
#endif
#ifdef USE_EVOI
    List<double> EVOI_list;
#endif
#ifdef USE_DMB
    List<double> DMB_list;
#endif
#ifdef USE_GTVol
    List<double> GTVol_list;
#endif
#ifdef USE_L2f1
    List<double> L2f1_list;
#endif
#ifdef USE_MPB
    List<double> MPB_list;
#endif
#ifdef USE_AsyBidAsk
    List<double> AsyBidAsk_list;
#endif
};

FastFactorMan::FastFactorMan(const std::string &name, const uint32_t &min_length)
    : TProduct{name}, m_MPBVS{0.5}, m_EVOI{0.8} {
    LOG_INFO1("[FastFactorMan] name:{} min_length:{}", name, min_length);
}
void FastFactorMan::guess_trade_price(const md::l2 &tick) {
    double left_amount = this->wvap * this->trade_vol;
    auto left_vol = this->trade_vol;
    // LOG_INFO5("left_amount:{:.3f} left_vol:{}", left_amount, left_vol);
    ask_vol = {0};
    bid_vol = {0};
    // int tmp_vol{0};
    bid_amount = {0};
    ask_amount = {0};
    int take_b1{0}, take_a1{0};  // take_lp{0};
    int gvp1{0};
    int ab_vol{0};
    double gp1{0};
    // double tmp_amount{0};
    // double tmp_a1{0}, tmp_b1{0};
    for (int i = 0; i < 5; ++i) {
        take_bid[i] = 0;
        take_ask[i] = 0;
    }

    // LOG_INFO5("Step1 bidv: {} askv: {} bm:{:.4f} am:{:.4f}", ask_vol, bid_vol, bid_amount, ask_amount);
    // Step 1. 假设 last_price成交了1手,
    // i. last_price 小于 mid_price, 定义为bid， ii. last_price 大于等于
    // mid_price，定义为 ask
    if (left_vol > 0) {
        left_vol -= 1;
        // take_lp = 1;
        if (Compare(tick.lastprice, mid_price) == -1) {
            bid_vol += 1;
            bid_amount += tick.lastprice;
        } else {
            ask_vol += 1;
            ask_amount += tick.lastprice;
        }
    } else {
        // DEBUG_GTP;
        return;
    }
    // CHECK_VOL_AMOUNT;

    // LOG_INFO5("Step2 bidv: {} askv: {} bm:{:.4f} am:{:.4f}", ask_vol, bid_vol, bid_amount, ask_amount);
    // Step 2. 假设 盘口扫过的都成交
    // LOG_INFO5("bid price: {:.2f}, {:.2f}, {:.2f}, {:.2f}, {:.2f}", tick.bid[0], tick.bid[1], tick.bid[2],
    // tick.bid[3],  tick.bid[4]);
    // LOG_INFO("bid price: {:.2f}, {:.2f}, {:.2f}, {:.2f}, {:.2f}", bid_pr[0], bid_pr[1], bid_pr[2], bid_pr[3],
    // bid_pr[4]);
    for (int i = 0; i < 5; ++i) {
        if (tick.bid[0] < bid_pr[i] && left_vol > 0) {
            // take_bid
            take_bid[i] = bidv_pr[i] <= left_vol ? bidv_pr[i] : left_vol;
            bid_vol += take_bid[i];
            bid_amount += take_bid[i] * bid_pr[i];
            left_vol -= take_bid[i];
        }
        // take ask
        if (tick.ask[0] > ask_pr[i] && left_vol > 0) {
            take_ask[i] = askv_pr[i] <= left_vol ? askv_pr[i] : left_vol;
            ask_vol += take_ask[i];
            ask_amount += take_ask[i] * ask_pr[i];
            left_vol -= take_ask[i];
        }
    }
    left_amount -= ask_amount + bid_amount;
    // CHECK_VOL_AMOUNT;
    // LOG_INFO5("Step3 bidv: {} askv: {} bm:{:.4f} am:{:.4f}", ask_vol, bid_vol, bid_amount, ask_amount);
    if (Compare(fabs(left_amount), 0) == 0) {
        return;
    }

    // Step 3. 将剩余的成交量分到 bid1, ask1
    ab_vol = left_vol;
    if (ab_vol > 0) {
        if (tick.ask[0] * ab_vol - left_amount < 0) {
            // all on ask
            take_a1 = ab_vol;
        } else if (left_amount - tick.bid[0] * ab_vol < 0) {
            // all on bid
            take_b1 = ab_vol;
        } else {
            // both on ask and bid
            take_a1 = hft::Round((left_amount - tick.bid[0] * ab_vol) / this->spread);
            take_b1 = hft::Round((tick.ask[0] * ab_vol - left_amount) / this->spread);
        }
        left_amount -= take_a1 * tick.ask[0] + take_b1 * tick.bid[0];
        left_vol -= take_b1 + take_a1;
        ask_amount += take_a1 * tick.ask[0];
        bid_amount += take_b1 * tick.bid[0];
        ask_vol += take_a1;
        bid_vol += take_b1;
    }
    // CHECK_VOL_AMOUNT;
    if (Compare(fabs(left_amount), 0) == 0) {
        // DEBUG_GTP;
        return;
    }
    // LOG_INFO5("Step4 bidv: {} askv: {} bm:{:.4f} am:{:.4f}", ask_vol, bid_vol, bid_amount, ask_amount);

    // Step 4. 将多余的一手就近分到ask1或bid1上
    // near ask
    if (left_vol != 0) {
        if (((left_vol == 1 && tick.ask[0] - left_amount < left_amount - tick.bid[0] + 0.01) ||
             (left_vol == -1 && tick.ask[0] + left_amount < -left_amount - tick.bid[0] + 0.01)) &&
            left_vol + take_a1 >= 0) {
            take_a1 += left_vol;
            ask_vol += left_vol;
            left_amount -= left_vol * tick.ask[0];
            ask_amount += tick.ask[0] * left_vol;
            left_vol = 0;
        } else {
            // near bid
            take_b1 += left_vol;
            bid_vol += left_vol;
            bid_amount += tick.bid[0] * left_vol;
            left_amount -= left_vol * tick.bid[0];
            left_vol = 0;
        }
    }
    // CHECK_VOL_AMOUNT;

    if (Compare(fabs(left_amount), 0) == 0) {
        return;
    }
    // LOG_INFO5("Step5 bidv: {} askv: {} bm:{:.4f} am:{:.4f}", ask_vol, bid_vol, bid_amount, ask_amount);
    // Step 5. 将多余的成交额分到bid1, ask1...
    if (take_b1 > 0) {
        take_b1 -= 1;
        gp1 = tick.bid[0] + left_amount;
        gvp1 = 1;
        left_amount -= left_amount;
        bid_vol -= 1;
        bid_amount -= tick.bid[0];
    } else if (take_a1 > 0) {
        take_a1 -= 1;
        gp1 = tick.ask[0] + left_amount;
        gvp1 = 1;
        left_amount -= left_amount;
        ask_vol -= 1;
        ask_amount -= tick.ask[0];
    } else {
        for (int i = 0; i < 5; ++i) {
            if (take_bid[i] > 0 && Compare(left_amount, 0) != 0) {
                take_bid[i] -= 1;
                bid_vol -= 1;
                bid_amount -= bid_pr[i];
                gvp1 = 1;
                gp1 = bid_pr[i] + left_amount;
                left_amount -= left_amount;
            }
        }
        for (int i = 0; i < 5; ++i) {
            if (take_ask[i] > 0 && Compare(left_amount, 0) != 0) {
                take_ask[i] -= 1;
                ask_amount -= ask_pr[i];
                ask_vol -= 1;
                gvp1 = 1;
                gp1 = ask_pr[i] + left_amount;
                left_amount -= left_amount;
            }
        }
    }
    if (Compare(gp1, mid_price) >= 0) {
        ask_vol += gvp1;
        ask_amount += gp1;
    } else {
        bid_vol += gvp1;
        bid_amount += gp1;
    }
}
void FastFactorMan::process_first_tick(const md::l2 &tick) {
    amount_pr = tick.lastamount;
    volume_pr = tick.lastvolume;
    mid_price_pr = 0.5 * (tick.ask[0] + tick.bid[0]);
    LOG_INFO1("[first tick] amount:{} volume:{}", amount_pr, volume_pr);
}
void FastFactorMan::init_data(md::l2 *tick_col, const uint32_t &length) {
    // process first tick
    // wvap = tp_list.pre(0);
    // mid_price_pr = mp.pre(0);
    // wvap = (*turn_over_itr) / trade_vol / multiplier;
    process_first_tick(tick_col[0]);
    for (uint32_t i = 1; i < length; ++i) {
        auto &tmp_tick = tick_col[i];
        eval(tmp_tick);
        ExPose(tmp_tick);
    }
}
void FastFactorMan::ExPose(const md::l2 &tick) {
    amount_pr = tick.lastamount;
    volume_pr = tick.lastvolume;
    mid_price_pr = mid_price;
    ask1_pr = tick.ask[0];
    bid1_pr = tick.bid[0];
    askv1_pr = tick.askv[0];
    bidv1_pr = tick.bidv[0];

    wvap_list.Append(wvap);
    mid_price_list.Append(mid_price);
    PIM_list.Append(PIM);
    ADI_list.Append(ADI);
    ask_amount_list.Append(ask_amount);
    bid_amount_list.Append(bid_amount);
    PIMsp_list.Append(PIMsp);
    ask5_list.Append(ask5);
    bid5_list.Append(bid5);
    pa_list.Append(pa);
    pb_list.Append(pb);
#ifdef USE_MPB
    MPB_list.Append(MPB);
#endif
#ifdef USE_MPBVS
    MPBVS_list.Append(MPBVS);
#endif
#ifdef USE_VPI
    VPI_list.Append(VPI);
#endif
#ifdef USE_VOI
    VOI_list.Append(VOI);
#endif
#ifdef USE_EVOI
    EVOI_list.Append(EVOI);
#endif
#ifdef USE_GTVol
    GTVol_list.Append(GTVol);
#endif
#ifdef USE_L2f1
    L2f1_list.Append(L2f1);
#endif
    ADI_pr = ADI;
#ifdef USE_DMB
    DMB_list.Append(DMB);
#endif
#ifdef USE_AsyBidAsk
    AsyBidAsk_list.Append(AsyBidAsk);
#endif

    for (int i = 0; i < 5; ++i) {
        bid_pr[i] = tick.bid[i];
        ask_pr[i] = tick.ask[i];
        bidv_pr[i] = tick.bidv[i];
        askv_pr[i] = tick.askv[i];
    }
}
void FastFactorMan::ReSet() {
    ask_amount_list.ReSet();
    bid_amount_list.ReSet();
    wvap_list.ReSet();
    mid_price_list.ReSet();
    PIM_list.ReSet();
    PIMsp_list.ReSet();
    ask5_list.ReSet();
    bid5_list.ReSet();
    pa_list.ReSet();
    pb_list.ReSet();
#ifdef USE_DMB
    DMB_list.ReSet();
#endif
#ifdef USE_VOI
    VOI_list.ReSet();
#endif
#ifdef USE_EVOI
    m_EVOI.ReSet();
    EVOI_list.ReSet();
#endif
#ifdef USE_ADI
    ADI_list.ReSet();
#endif
#ifdef USE_GTVol
    GTVol_list.ReSet();
#endif
#ifdef USE_L2f1
    L2f1_list.ReSet();
#endif
#ifdef USE_MPB
    MPB_list.ReSet();
#endif
#ifdef USE_MPBVS
    m_MPBVS.ReSet();
    MPBVS_list.ReSet();
#endif
#ifdef USE_VPI
    VPI_list.ReSet();
#endif
}
void FastFactorMan::eval(const md::l2 &tick) {
    turn_over = tick.lastamount - amount_pr;
    trade_vol = tick.lastvolume - volume_pr;
    wvap = trade_vol > 0 ? turn_over / trade_vol / multiplier : tick.lastprice;
    mid_price = 0.5 * (tick.ask[0] + tick.bid[0]);
    pa = tick.ask[4] - mid_price;
    ask1 = tick.ask[0];
    ask5 = tick.ask[4];
    pb = mid_price - tick.bid[4];
    bid1 = tick.bid[0];
    bid5 = tick.bid[4];
    spread = tick.ask[0] - tick.bid[0];
    PIM = wvap - 0.5 * (mid_price + mid_price_pr);
    PIMsp = PIM / spread;
    askv1 = tick.askv[0];
    sum_askv = tick.askv[0] + tick.askv[1] + tick.askv[2] + tick.askv[3] + tick.askv[4];
    bidv1 = tick.bidv[0];
    sum_bidv = tick.bidv[0] + tick.bidv[1] + tick.bidv[2] + tick.bidv[3] + tick.bidv[4];
#ifdef USE_ADI
    ADI = static_cast<double>(sum_bidv - sum_askv) / (sum_bidv + sum_askv);
#endif

#ifdef USE_DMB
    DMB = (-2 * tick.ask[0] * tick.bid[0] + (tick.ask[0] + tick.bid[0]) * wvap) / (wvap * spread);
#endif

#ifdef USE_MPB
    MPB = wvap - 0.5 * (mid_price + mid_price_pr);
#endif
#ifdef USE_MPBVS
    auto mpbvs = (wvap - mid_price) * sqrt(trade_vol * spread * (bidv1 + askv1));
    m_MPBVS.Add(mpbvs);
    MPBVS = m_MPBVS.GetMean();
#endif
    guess_trade_price(tick);
#ifdef USE_VPI
    VPI = static_cast<double>(ask_vol - bid_vol) / (ask_vol + bid_vol);
#endif

#ifdef USE_VOI
    VOI = GetVOI();
#endif
#ifdef USE_EVOI
    VOI = GetVOI();
    m_EVOI.Add(VOI);
    EVOI = GetEVOI();
#endif
#ifdef USE_GTVol
    GTVol = GetGTVol();
#endif
#ifdef USE_L2f1
    L2f1 = GetL2f1(tick);
#endif
#ifdef USE_AsyBidAsk
    AsyBidAsk = GetAsyBidAsk(tick);
#endif
}
double FastFactorMan::GetVOI() {
    int32_t delta_bidv1{0}, delta_askv1{0};
    if (Compare(ask1, ask1_pr) == 0) {
        delta_askv1 = askv1 - askv1_pr;
    } else if (Compare(ask1, ask1_pr) == -1) {
        delta_askv1 = askv1;
    }
    if (Compare(bid1, bid1_pr) == 1) {
        delta_bidv1 = bidv1;
    } else if (Compare(bid1, bid1_pr) == 0) {
        delta_bidv1 = bidv1 - bidv1_pr;
    }
    return delta_bidv1 - delta_askv1;
}
double FastFactorMan::GetAsyBidAsk(const md::l2 &tick) {
    return log((-tick.ask[0] + tick.ask[4]) / (tick.bid[0] - tick.bid[4]));
}
double FastFactorMan::GetGTVol() {
#ifdef USE_GTVol
    return trade_vol > 0 ? ask_vol - bid_vol : GTVol;
#else
    return 0;
#endif
}
#ifdef USE_EVOI
double FastFactorMan::GetEVOI() { return m_EVOI.GetMean(); }
#endif

double FastFactorMan::GetL2f1(const md::l2 &tick) {
    double tmp{0};
    for (int i = 0; i < 5; ++i) {
        tmp -= tick.ask[i] * tick.askv[i] - ask_pr[i] * askv_pr[i];
        tmp += tick.bid[i] * tick.bidv[i] - bid_pr[i] * bidv_pr[i];
    }
    tmp += ask_amount;
    tmp -= bid_amount;
    return tmp * 1e-5;
}
#ifdef USE_DMB
double FastFactorMan::a1f4() { return DMB - DMB_list.pre(0); }
#endif
#ifdef USE_VOI
double FastFactorMan::a1f13() { return VOI - VOI_list.pre(2); }
    // a1f13 = factor::get_a1(VOI_list, 3);
#endif
#ifdef USE_EVOI
double FastFactorMan::a1f5() { return EVOI - EVOI_list.pre(2); }
double FastFactorMan::a1f6() { return EVOI - EVOI_list.pre(3); }
#endif
#ifdef USE_GTVol
double FastFactorMan::a1f7() { return GTVol - GTVol_list.pre(2); }
    // a1f7 = factor::get_a1(GTVol_list, 3);
#endif
#ifdef USE_L2f1
double FastFactorMan::a1f9() { return L2f1 - L2f1_list.pre(3); }
// a1f9 = factor::get_a1(L2f1_list, 4);
double FastFactorMan::a2f13() { return L2f1 + L2f1_list.pre(5) - 2 * L2f1_list.pre(2); }
// a2f13 = factor::get_a2(L2f1_list, 3);
#endif
#ifdef USE_MPBVS
double FastFactorMan::a1f10() { return MPBVS - MPBVS_list.pre(0); }
    // a1f10 = factor::get_a1(MPBVS_list, 1);
#endif
#ifdef USE_MPB
double FastFactorMan::a1f11() { return MPB - MPB_list.pre(2); }
double FastFactorMan::a1f12() { return MPB - MPB_list.pre(3); }
    // a1f11 = factor::get_a1(MPB_list, 3);
    // a1f12 = factor::get_a1(MPB_list, 4);
#endif

#ifdef USE_VPI
double FastFactorMan::a1f14() { return VPI - VPI_list.pre(2); }
double FastFactorMan::a1f15() { return VPI - VPI_list.pre(3); }
// a1f14 = factor::get_a1(VPI_list, 3);
// a1f15 = factor::get_a1(VPI_list, 4);
#endif
#ifdef USE_AsyBidAsk
double FastFactorMan::a2f5() { return AsyBidAsk + AsyBidAsk_list.pre(3) - 2 * AsyBidAsk_list.pre(1); }
double FastFactorMan::a2f6() { return AsyBidAsk + AsyBidAsk_list.pre(5) - 2 * AsyBidAsk_list.pre(2); }
double FastFactorMan::a2f7() { return AsyBidAsk + AsyBidAsk_list.pre(7) - 2 * AsyBidAsk_list.pre(3); }
// a2f5 = factor::get_a2(AsyBidAsk_list, 2);
// a2f6 = factor::get_a2(AsyBidAsk_list, 3);
// a2f7 = factor::get_a2(AsyBidAsk_list, 4);
#endif
#ifdef USE_GTVol
// a2f12 = factor::get_a2(GTVol_list, 4);
double FastFactorMan::a2f12() { return GTVol + GTVol_list.pre(7) - 2 * GTVol_list.pre(3); }
#endif
// a2f13 = factor::get_a2(L2f1_list, 3);
}  // namespace fast
}  // namespace hft
