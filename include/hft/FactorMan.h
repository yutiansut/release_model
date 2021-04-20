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
class FactorMan : public MarketData, public TProduct {
   public:
    FactorMan(const std::string &name, const uint32_t &min_length);
    void ReSet();
    void Calculate() override;
    // 事后调用，用来优化性能
    virtual void ExPose();
    void SetDoCal(bool do_cal) { m_do_cal_factor = do_cal; }

   public:
    // factor
    double VOI() const { return m_VOI; }
    double EVOI() const { return m_EVOI.GetMean(); }
    double OIR() const { return m_OIR; }
    double VTP() const { return m_VTP; }
    double MPB() const { return m_MPB; }
    double TagReturn(const size_t &timeperiod) const;
    double MPBVS() const { return m_MPBVS.GetMean(); }
    double SignVol_1() const { return m_SignVol_1.GetMean(); }
    double Entropy_pl() const;
    double DMB() const { return m_DMB; }
    // double RSI_dev(const size_t &timeperiod) const;
    double SumSizeDiff() const { return m_sumsizediff; }
    double ADI() const { return m_ADI; }
    double Dispersion() const { return m_Dispersion; }
    double AsyBidAsk() const { return m_AsyBidAsk; }
    double hasbrouck_log_beta1(const size_t &timeperiod) const;
    double hasbrouck_log_beta2(const size_t &timeperiod) const;
    double hasbrouck_log_beta3(const size_t &timeperiod) const;
    double hasbrouck_log_beta4(const size_t &timeperiod) const;
    // double L2f2() const { return m_L2f2; }
    double VPI() const { return m_VPI; }
    double PID() const { return m_PID; }
    // double Ff4() const { return m_Ff4; }
    double GTVol() const { return m_GTVol; }
    void cal_Ff4() {
        if (UNLIKELY(mp.cum_size() < 3)) {
            if (mp.cum_size() == 1) {
                m_Ff4 = 0;
            } else {
                m_Ff4 = (mp.pre(0) - mp.pre(1)) / sp_list.pre(1);
            }
        } else {
            m_Ff4 = factor::Ff4(mp, sp_list, m_Ff4);
        }
    }
    void cal_GTVol() {
        // if (volume.pre(0) > 0) {
        //    m_GTVol = ask_vol - bid_vol;
        //}
        m_GTVol = volume.pre(0) > 0 ? ask_vol - bid_vol : m_GTVol;
        GTVol_list.Append(m_GTVol);
        LOG_INFO("[{}_GTVol] {}", name.c_str(), m_GTVol);
    }

   public:
    // intens factor
    double Hurst_factor(const size_t &timeperiod);
    double test(const uint16_t &timeperiod) {
        // auto tmp = last_price;
        // cal_Ff4();
        // auto var = (bid_vol_list + 10000).pre(0);
        auto var = factor::get_a1(AsyBidAsk_list, 1);
        LOG_INFO("[lp_mean] {}", var);
        LOG_INFO("[lp_end] {}", ask_vol_list.pre(0));
        return var;
    }

   public:
    // base factor
    double MP() { return mid_price; }
    double TP() { return tp; }
    double Spread() { return sp; }

   private:
    // 中间变量
    EWMCaler<double> m_EVOI, m_MPBVS, m_SignVol_1;
    double GetMP(const size_t &timeperiod);
    double m_Ff4;

   public:
    void SetEVOIAlpha(const double &alpha) { m_EVOI.SetAlpha(alpha); }
    void SetMPBVSAlpha(const double &alpha) { m_MPBVS.SetAlpha(alpha); }
    void SetSignVol_1Alpha(const double &alpha) { m_SignVol_1.SetAlpha(alpha); }

   private:
    // factor
    double m_VOI, m_OIR, m_VTP, m_MPB, m_DMB;
    double m_sumsizediff, m_ADI, m_Dispersion, m_AsyBidAsk;
    // double m_L2f2;
    double m_VPI, m_PID;
    double bid_amount, ask_amount;
    double m_GTVol;

   public:
    int32_t bid_vol, ask_vol;
    int32_t sab, sa2, sa3;
    double Ff1, Ff2, Ff3, Ff4, Ff5, Ff6, Ff7_10, Ff7_20, Ff7_40, Ff7_80, Ff7, Ff8_1, Ff8_2;
    double L2f1, L2f2, L2f6, L2f7, L2f8, L2f9, L2f10_10, L2f10_20, L2f10_40, L2f10_60, L2f10_80, L2f10, L2f11, L2f12,
        L2f13, L2f14, L2f15;
    double L2a1, L2a2, L2a3, L2a4, L2p1, L2p2;
    double ask_vol_ewma_1, bid_vol_ewma_1;
    double a1f1, a1f2, a1f3, a1f4, a1f5, a1f6, a1f7, a1f8, a1f9, a1f10, a1f11, a1f12, a1f13, a1f14, a1f15;
    double a2f1, a2f2, a2f3, a2f4, a2f5, a2f6, a2f7, a2f8, a2f9, a2f10, a2f11, a2f12, a2f13, a2f14, a2f15, a2f16, a2f17,
        a2f18, a2f19, a2f20, a2f21, a2f22, a2f23, a2f24, a2f25, a2f26, a2f27, a2f28, a2f29, a2f30;
    double Tf1, Tf2, Tf3, Tf4, Tf5, Tf6, Tf7;

   private:
    int32_t m_sum_bidv, m_sum_askv;
    int32_t take_bid[5], take_ask[5];
    // at least 2 ticks
   public:
    List<int32_t> bid_vol_list;
    List<int32_t> ask_vol_list;
    List<double> bid_amount_list;
    List<double> ask_amount_list;
    List<double> AsyBidAsk_list;
    List<double> ADI_list;
    List<double> ADI_rank_5;
    List<double> ADI_rank_6;
    List<double> ADI_rank_6_rank_80;
    List<double> SumSizeDiff_list;
    List<double> DMB_list;
    List<double> EVOI_list;
    List<double> VOI_list;
    List<double> VPI_list;
    List<double> GTVol_list;
    List<float> L2f1_list;
    List<double> MPBVS_list;
    List<double> MPB_list;
    List<double> SignVol_1_list;

   private:
    bool m_do_cal_factor;
    void cal_VOI();
    // at least 1 ticks
    void cal_OIR();
    // at least 1 ticks
    void cal_VTP();
    // at least 2 ticks
    void cal_MPB();
    void cal_TagReturn(const size_t &timeperiod);
    // at least 3 ticks
    void cal_MPBVS();
    void cal_SignVol_1();
    void cal_Entropy_pl();
    void cal_DMB();
    void cal_RSI_dev(const size_t &timeperiod);
    // at least 1 ticks
    void cal_SumSizeDiff();
    // at least 1 ticks
    void cal_ADI();
    // at least 1 ticks
    void cal_Dispersion();
    // at least 1 ticks
    void cal_AsyBidAsk();
    void cal_L2f2();
    void guess_trade_price();
    void cal_ABvol();
    void cal_hasbrouck_log_beta1(const size_t &timeperiod);
    void cal_hasbrouck_log_beta2(const size_t &timeperiod);
    void cal_hasbrouck_log_beta3(const size_t &timeperiod);
    void cal_hasbrouck_log_beta4(const size_t &timeperiod);
    void cal_factors() {
        LOG_INFO1("[FactorMan::{}]", __FUNCTION__);
        LOG_INFO1("[{}_wvap] {}", this->name.c_str(), tp);

#ifdef CAL_Ff1
        this->Ff1 = factor::Ff1(bid[0], volume);
        LOG_INFO("[{}_Ff1] {}", this->name, Ff1);
#endif
#ifdef CAL_Ff2
        this->Ff2 = factor::Ff2(bid[0], bid_vol_list);
        LOG_INFO("[{}_Ff2] {}", this->name, Ff2);
#endif
#ifdef CAL_Ff3
        this->Ff3 = factor::Ff3(bid[0], bid_vol_list);
        LOG_INFO("[{}_Ff3] {}", this->name, Ff3);
#endif
#ifdef CAL_Ff4
        if (UNLIKELY(TickeLength() <= 2)) {
            this->Ff4 = factor::Ff4(mp, sp_list, 0, 1.0);
            LOG_INFO("[{}_Ff4] {}", this->name, Ff4);
        } else {
            this->Ff4 = factor::Ff4(mp, sp_list, Ff4, 2.0 / 3);
            LOG_INFO("[{}_Ff4] {}", this->name, Ff4);
        }
#endif
#ifdef CAL_Ff5
        this->Ff5 = factor::Ff5(mp, last_price);
        LOG_INFO("[{}_Ff5] {}", this->name, Ff5);
#endif
#ifdef CAL_Ff6
        this->Ff6 = factor::Ff6(tp_list, mp, sp_list);
        LOG_INFO("[{}_Ff6] {}", this->name, Ff6);
#endif
#ifdef CAL_Ff7_10
        this->Ff7_10 = factor::Ff7_10(tp_list, mp, sp_list);
        LOG_INFO("[{}_Ff7_10] {}", this->name, Ff7_10);
#endif
#ifdef CAL_Ff7_20
        this->Ff7_20 = factor::Ff7_20(tp_list, mp, sp_list);
        LOG_INFO("[{}_Ff7_20] {}", this->name, Ff7_20);
#endif
#ifdef CAL_Ff7_40
        this->Ff7_40 = factor::Ff7_40(tp_list, mp, sp_list);
        LOG_INFO("[{}_Ff7_40] {}", this->name, Ff7_40);
#endif
#ifdef CAL_Ff7_80
        this->Ff7_80 = factor::Ff7_80(tp_list, mp, sp_list);
        LOG_INFO("[{}_Ff7_80] {}", this->name, Ff7_80);
#endif
#ifdef CAL_Ff7
        this->Ff7 = factor::Ff7(tp_list, mp, sp_list);
        LOG_INFO("[{}_Ff7] {}", this->name, Ff7);
#endif
#ifdef CAL_Ff8_1
        this->Ff8_1 = factor::Ff8_1(tp_list, mp, sp_list);
        LOG_INFO("[{}_Ff8_1] {}", this->name, Ff8_1);
#endif
#ifdef CAL_Ff8_2
        this->Ff8_2 = factor::Ff8_2(tp_list, mp, sp_list);
        LOG_INFO("[{}_Ff8_2] {}", this->name, Ff8_2);
#endif

#if defined(CAL_L2f1) || defined(CAL_a1f9) || defined(CAL_a2f13) || defined(CAL_a2f14)
        this->L2f1 = factor::L2f1(ask, askv, bid, bidv, bid_amount_list, ask_amount_list);
        LOG_INFO("[{}_L2f1] {}", this->name, L2f1);
        L2f1_list.Append(L2f1);
#endif
#ifdef CAL_L2f2
        cal_L2f2();
        LOG_INFO("[{}_L2f2] {}", name.c_str(), L2f2);
#endif

#ifdef CAL_L2f6
        this->L2f6 = factor::L2f6(ask_vol_list, bid_vol_list);
        LOG_INFO("[{}_L2f6] {}", name.c_str(), L2f6);
#endif

#ifdef CAL_L2f7
        this->L2f7 = factor::L2f7(askv, bidv);
        LOG_INFO("[{}_L2f7] {}", name.c_str(), L2f7);
#endif

#ifdef CAL_L2f8
        this->L2f8 = factor::L2f8(ask[4], bid[4], mp);
        LOG_INFO("[{}_L2f8] {}", name, L2f8);
#endif

#ifdef CAL_L2f9
        this->L2f9 = factor::L2f9(ask[4], bid[4]);
        LOG_INFO("[{}_L2f9] {}", name, L2f9);
#endif

#ifdef CAL_L2f10_10
        this->L2f10_10 = factor::L2f10(ask_amount_list, bid_amount_list, 10);
        LOG_INFO("[{}_L2f10_10] {}", this->name, L2f10_10);
#endif
#ifdef CAL_L2f10_20
        this->L2f10_20 = factor::L2f10(ask_amount_list, bid_amount_list, 20);
        LOG_INFO("[{}_L2f10_20] {}", this->name, L2f10_20);
#endif

#ifdef CAL_L2f10_40
        this->L2f10_40 = factor::L2f10(ask_amount_list, bid_amount_list, 40);
        LOG_INFO("[{}_L2f10_40] {}", this->name, L2f10_40);
#endif

#ifdef CAL_L2f10_60
        this->L2f10_60 = factor::L2f10(ask_amount_list, bid_amount_list, 60);
        LOG_INFO("[{}_L2f10_60] {}", this->name, L2f10_60);
#endif

#ifdef CAL_L2f10_80
        this->L2f10_80 = factor::L2f10(ask_amount_list, bid_amount_list, 80);
        LOG_INFO("[{}_L2f10_80] {}", this->name, L2f10_80);
        LOG_INFO("[{}_ask_amount] {}", this->name, ask_amount_list.pre(0));
        LOG_INFO("[{}_bid_amount] {}", this->name, bid_amount_list.pre(0));
        // LOG_INFO("[{}_ask_vol] {}", this->name, ask_vol_list.pre(0));
#endif

#ifdef CAL_L2f10
        this->L2f10 =
            0.5 *
            (factor::L2f10(ask_amount_list, bid_amount_list, 10) + factor::L2f10(ask_amount_list, bid_amount_list, 20) +
             factor::L2f10(ask_amount_list, bid_amount_list, 40) + factor::L2f10(ask_amount_list, bid_amount_list, 80));
        LOG_INFO("[{}_L2f10] {}", this->name, this->L2f10);
#endif

#ifdef CAL_L2f11
        this->L2f11 = factor::L2f11(ask_vol_list, bid_vol_list);
        LOG_INFO("[{}_L2f11] {}", this->name, this->L2f11);
#endif
#ifdef CAL_L2f12
        this->L2f12 = factor::L2f12(ask_amount_list, bid_amount_list);
        LOG_INFO("[{}_L2f12] {}", this->name, this->L2f12);
#endif

#ifdef CAL_L2f13
        this->L2f13 = factor::L2f13(ask_vol_list, bid_vol_list);
        LOG_INFO("[{}_L2f13] {}", this->name, this->L2f13);
#endif

#ifdef CAL_L2f14
        this->L2f14 = factor::L2f14(ask_vol_list, bid_vol_list);
        LOG_INFO("[{}_L2f14] {}", this->name, this->L2f14);
#endif
#ifdef CAL_L2f15
        auto A = ff::ts_rank(ask_vol_list, 40) / bid_vol;
        auto B = ff::ts_rank(bid_vol_list, 40) / ask_vol;
        this->L2f15 = A - B;
        LOG_INFO("[{}_L2f15] {}", this->name, this->L2f15);
#endif

#ifdef CAL_L2a1
        this->L2a1 = factor::L2a1(ADI_list);
        LOG_INFO1("[{}_L2a1] {}", this->name, this->L2a1);
#endif

#ifdef CAL_L2a2
        this->L2a2 = factor::L2a2(ADI_list);
        LOG_INFO("[{}_L2a2] {}", this->name, this->L2a2);
#endif

#ifdef CAL_L2a3
        this->L2a3 = factor::L2a3(ADI_rank_5);
        LOG_INFO("[{}_L2a3] {}", this->name, this->L2a3);
#endif

#ifdef CAL_L2a4
        this->L2a4 = factor::L2a4(ADI_rank_6_rank_80);
        LOG_INFO("[{}_L2a4] {}", this->name, this->L2a4);
#endif

#ifdef CAL_L2p1
        this->L2p1 = factor::L2p1(ask[4], bid[4], mp);
        LOG_INFO("[{}_L2p1] {}", this->name, this->L2p1);
#endif

#ifdef CAL_L2p2
        this->L2p2 = factor::L2p2(ask[4], bid[4], mp);
        LOG_INFO("[{}_L2p2] {}", this->name, this->L2p2);
#endif

#ifdef CAL_a1f1
        a1f1 = factor::get_a1(ADI_list, 1);
        LOG_INFO("[{}_a1f1] {}", name, a1f1);
#endif

#ifdef CAL_a1f2
        a1f2 = factor::get_a1(AsyBidAsk_list, 1);
        LOG_INFO("[{}_a1f2] {}", name, a1f2);
#endif

#ifdef CAL_a1f3
        a1f3 = factor::get_a1(SumSizeDiff_list, 1);
        LOG_INFO("[{}_a1f3] {}", name, a1f3);
#endif

#ifdef CAL_a1f4
        a1f4 = factor::get_a1(DMB_list, 1);
        LOG_INFO("[{}_a1f4] {}", name, a1f4);
#endif

#ifdef CAL_a1f5
        a1f5 = factor::get_a1(EVOI_list, 3);
        LOG_INFO("[{}_a1f5] {}", name, a1f5);
#endif

#ifdef CAL_a1f6
        a1f6 = factor::get_a1(EVOI_list, 4);
        LOG_INFO("[{}_a1f6] {}", name, a1f6);
#endif

#ifdef CAL_a1f7
        a1f7 = factor::get_a1(GTVol_list, 3);
        LOG_INFO("[{}_a1f7] {}", name, a1f7);
#endif

#ifdef CAL_a1f8
        a1f8 = factor::get_a1(GTVol_list, 4);
        LOG_INFO("[{}_a1f8] {}", name, a1f8);
#endif

#ifdef CAL_a1f9
        a1f9 = factor::get_a1(L2f1_list, 4);
        LOG_INFO("[{}_a1f9] {}", name, a1f9);
#endif

#ifdef CAL_a1f10
        a1f10 = factor::get_a1(MPBVS_list, 1);
        LOG_INFO("[{}_a1f10] {}", name, a1f10);
#endif

#ifdef CAL_a1f11
        a1f11 = factor::get_a1(MPB_list, 3);
        LOG_INFO("[{}_a1f11] {}", name, a1f11);
#endif

#ifdef CAL_a1f12
        a1f12 = factor::get_a1(MPB_list, 4);
        LOG_INFO("[{}_a1f12] {}", name, a1f12);
#endif

#ifdef CAL_a1f13
        a1f13 = factor::get_a1(VOI_list, 3);
        LOG_INFO("[{}_a1f13] {}", name, a1f13);
#endif

#ifdef CAL_a1f14
        a1f14 = factor::get_a1(VPI_list, 3);
        LOG_INFO("[{}_a1f14] {}", name, a1f14);
#endif

#ifdef CAL_a1f15
        a1f15 = factor::get_a1(VPI_list, 4);
        LOG_INFO("[{}_a1f15] {}", name, a1f15);
#endif

#ifdef CAL_a2f1
        // G2_a2
        a2f1 = factor::get_a2(ADI_list, 2);
        LOG_INFO("[{}_a2f1] {}", name, a2f1);
#endif

#ifdef CAL_a2f2
        a2f2 = factor::get_a2(VPI_list, 5);
        LOG_INFO("[{}_a2f2] {}", name, a2f2);
#endif

#ifdef CAL_a2f3
        a2f3 = factor::get_a2(ADI_list, 3);
        LOG_INFO("[{}_a2f3] {}", name, a2f3);
#endif

#ifdef CAL_a2f4
        a2f4 = factor::get_a2(AsyBidAsk_list, 1);
        LOG_INFO("[{}_a2f4] {}", name, a2f4);
#endif

#ifdef CAL_a2f5
        a2f5 = factor::get_a2(AsyBidAsk_list, 2);
        LOG_INFO("[{}_a2f5] {}", name, a2f5);
#endif

#ifdef CAL_a2f6
        a2f6 = factor::get_a2(AsyBidAsk_list, 3);
        LOG_INFO("[{}_a2f6] {}", name, a2f6);
#endif

#ifdef CAL_a2f7
        a2f7 = factor::get_a2(AsyBidAsk_list, 4);
        LOG_INFO("[{}_a2f7] {}", name, a2f7);
#endif

#ifdef CAL_a2f8
        a2f8 = factor::get_a2(DMB_list, 7);
        LOG_INFO("[{}_a2f8] {}", name, a2f8);
#endif

#ifdef CAL_a2f9
        a2f9 = factor::get_a2(EVOI_list, 2);
        LOG_INFO("[{}_a2f9] {}", name, a2f9);
#endif

#ifdef CAL_a2f10
        a2f10 = factor::get_a2(EVOI_list, 3);
        LOG_INFO("[{}_a2f10] {}", name, a2f10);
#endif

#ifdef CAL_a2f11
        a2f11 = factor::get_a2(GTVol_list, 3);
        LOG_INFO("[{}_a2f11] {}", name, a2f11);
#endif

#ifdef CAL_a2f12
        a2f12 = factor::get_a2(GTVol_list, 4);
        LOG_INFO("[{}_a2f12] {}", name, a2f12);
#endif

#ifdef CAL_a2f13
        a2f13 = factor::get_a2(L2f1_list, 3);
        LOG_INFO("[{}_a2f13] {}", name, a2f13);
#endif

#ifdef CAL_a2f14
        a2f14 = factor::get_a2(L2f1_list, 4);
        LOG_INFO("[{}_a2f14] {}", name, a2f14);
#endif

#ifdef CAL_a2f15
        a2f15 = factor::get_a2(MPBVS_list, 5);
        LOG_INFO("[{}_a2f15] {}", name, a2f15);
#endif

#ifdef CAL_a2f16
        a2f16 = factor::get_a2(MPBVS_list, 6);
        LOG_INFO("[{}_a2f16] {}", name, a2f16);
#endif

#ifdef CAL_a2f17
        a2f17 = factor::get_a2(MPB_list, 3);
        LOG_INFO("[{}_a2f17] {}", name, a2f17);
#endif

#ifdef CAL_a2f18
        a2f18 = factor::get_a2(MPB_list, 4);
        LOG_INFO("[{}_a2f18] {}", name, a2f18);
#endif

#ifdef CAL_a2f19
        a2f19 = factor::get_a2(MPB_list, 5);
        LOG_INFO("[{}_a2f19] {}", name, a2f19);
#endif

#ifdef CAL_a2f21
        // LOG_INFO("[a2f20] {}", a2f20);
        a2f21 = factor::get_a2(SignVol_1_list, 1);
        LOG_INFO("[{}_a2f21] {}", name, a2f21);
#endif

#ifdef CAL_a2f22
        a2f22 = factor::get_a2(SignVol_1_list, 3);
        LOG_INFO("[{}_a2f22] {}", name, a2f22);
#endif

#ifdef CAL_a2f23
        a2f23 = factor::get_a2(SignVol_1_list, 4);
        LOG_INFO("[{}_a2f23] {}", name, a2f23);
#endif

#ifdef CAL_a2f24
        a2f24 = factor::get_a2(SumSizeDiff_list, 2);
        LOG_INFO("[{}_a2f24] {}", name, a2f24);
#endif

#ifdef CAL_a2f25
        a2f25 = factor::get_a2(SumSizeDiff_list, 3);
        LOG_INFO("[{}_a2f25] {}", name, a2f25);
#endif

#ifdef CAL_a2f26
        a2f26 = factor::get_a2(tp_list, 2);
        LOG_INFO("[{}_a2f26] {}", name, a2f26);
#endif

#ifdef CAL_a2f27
        a2f27 = factor::get_a2(VOI_list, 3);
        LOG_INFO("[{}_a2f27] {}", name, a2f27);
#endif

#ifdef CAL_a2f28
        a2f28 = factor::get_a2(VOI_list, 4);
        LOG_INFO("[{}_a2f28] {}", name, a2f28);
#endif

#ifdef CAL_a2f29
        a2f29 = factor::get_a2(VPI_list, 3);
        LOG_INFO("[{}_a2f29] {}", name, a2f29);
#endif

#ifdef CAL_a2f30
        a2f30 = factor::get_a2(VPI_list, 4);
        LOG_INFO("[{}_a2f30] {}", name, a2f30);
#endif
#ifdef CAL_Tf1
        Tf1 = hft::ff::Tf1(mp, 40);
        LOG_INFO("[{}_Tf1] {}", name, Tf1);
#endif
#ifdef CAL_Tf2
        Tf2 = hft::ff::Tf2(mp, 40);
        LOG_INFO("[{}_Tf2] {}", name, Tf2);
#endif
#ifdef CAL_Tf3
        Tf3 = hft::ff::Tf3(ask_vol_list, bid_vol_list, 20);
        LOG_INFO("[{}_Tf3] {}", name, Tf3);
#endif
#ifdef CAL_Tf4
        Tf4 = hft::ff::Tf4(ask_vol_list, 20);
        LOG_INFO("[{}_Tf4] {}", name, Tf4);
#endif
#ifdef CAL_Tf5
        Tf5 = hft::ff::Tf5(ask_vol_list, 20);
        LOG_INFO("[{}_Tf5] {}", name, Tf5);
#endif
#ifdef CAL_Tf6
        Tf6 = hft::ff::Tf6(bid_vol_list, 20);
        LOG_INFO("[{}_Tf6] {}", name, Tf6);
#endif
#ifdef CAL_Tf7
        Tf7 = hft::ff::Tf7(bid_vol_list, 20);
        LOG_INFO("[{}_Tf7] {}", name, Tf7);
#endif
#ifdef CAL_sa2
        sa2 = math_tool::sign(tp_list.pre(0) - tp_list.pre(2));
        // HUSKY_TRADE_INFO("tp_0:{} tp_1:{} sign:{}", tp_list.pre(0), tp_list.pre(1), sa2);
#endif
#ifdef CAL_sa3
        sa3 = math_tool::sign(tp_list.pre(0) - tp_list.pre(3));
        // HUSKY_TRADE_INFO("tp_0:{} tp_2:{} sign:{}", tp_list.pre(0), tp_list.pre(1), sa3);
#endif
    }
};
FactorMan::FactorMan(const std::string &name, const uint32_t &min_length)
    : MarketData{min_length},
      TProduct{name},
      m_EVOI{0.8},
      m_MPBVS{0.5},
      m_SignVol_1{0.9},
      bid_vol_list{min_length},
      ask_vol_list{min_length},
      bid_amount_list{min_length},
      ask_amount_list{min_length},
      AsyBidAsk_list{min_length},
      ADI_list{min_length},
      ADI_rank_5{min_length},
      ADI_rank_6{min_length},
      ADI_rank_6_rank_80{min_length},
      SumSizeDiff_list{min_length},
      DMB_list{min_length},
      EVOI_list{min_length},
      VOI_list{min_length},
      VPI_list{min_length},
      GTVol_list{min_length},
      L2f1_list{min_length},
      MPBVS_list{min_length},
      MPB_list{min_length},
      SignVol_1_list{min_length} {
    // printf("%s %d\n", __FUNCTION__, __LINE__);
    SetMultiplier(TProduct::multiplier);
    this->SetDoCal(false);
}
void FactorMan::ReSet() {
    MarketData::ReSet();
    m_EVOI.ReSet();
    m_MPBVS.ReSet();
    m_SignVol_1.ReSet();
    ADI_list.ReSet();
    ADI_rank_5.ReSet();
    ADI_rank_6.ReSet();
    ADI_rank_6_rank_80.ReSet();
}
void FactorMan::Calculate() {
    LOG_INFO1("[FactorMan::{}]", __FUNCTION__);
    if (UNLIKELY(last_price.cum_size() < 2)) {
        cal_ABvol();
        // printf("do calculation: %u \n", bid_vol);
        // double alphe_1 = 1.0 / (1 + 1);
        // ask_vol_ewma_1 =
        // test(10);
        if (m_do_cal_factor) {
            cal_OIR();
            cal_VTP();
            cal_MPBVS();
            cal_DMB();
            LOG_INFO("[{}_MPB] {}", name, 0);
            cal_SumSizeDiff();
            cal_ADI();
            cal_Dispersion();
            cal_AsyBidAsk();
            // printf("do calculation: %s \n", this->name.c_str());
            cal_factors();
        }
        LOG_INFO("[{}_EVOI] {}", name, 0);
        LOG_INFO("[{}_VOI] {}", name, 0);
        LOG_INFO("[{}_GTVol] {}", name, 0);
        LOG_INFO("[{}_SignVol_1] {}", name, 0);
    } else {
        cal_ABvol();
        if (m_do_cal_factor) {
            cal_VOI();
            cal_OIR();
            cal_VTP();
            cal_MPB();
            cal_MPBVS();
            cal_SignVol_1();
            cal_DMB();
            cal_SumSizeDiff();
            cal_ADI();
            cal_Dispersion();
            cal_AsyBidAsk();
            // test(10);
            cal_GTVol();
            // printf("do calculation: %u \n", bid_vol);
            cal_factors();
        }
    }
    LOG_INFO("[{}_multiplier {}]", name, multiplier);
}
void FactorMan::ExPose() {}
double FactorMan::GetMP(const size_t &timeperiod) { return math_tool::get_mean(mp_itr - timeperiod, mp_itr); }
void FactorMan::cal_VOI() {
#if defined(CAL_VOI) || defined(CAL_EVOI)
    int32_t delta_bidv1{0}, delta_askv1{0};
    switch (Compare(*(bid_itr[0]), *(bid_itr[0] - 1))) {
        case 1:
            delta_bidv1 = *(bidv_itr[0]);
            break;
        case 0:
            delta_bidv1 = *(bidv_itr[0]) - *(bidv_itr[0] - 1);
            break;
        default:
            break;
    }
    switch (Compare(*(ask_itr[0]), *(ask_itr[0] - 1))) {
        case 0:
            delta_askv1 = *(askv_itr[0]) - *(askv_itr[0] - 1);
            break;
        case -1:
            delta_askv1 = *(askv_itr[0]);
            break;
        default:
            break;
    }
    m_VOI = delta_bidv1 - delta_askv1;
    VOI_list.Append(m_VOI);
    LOG_INFO("[{}_VOI] {}", name, m_VOI);
    m_EVOI.Add(m_VOI);
    LOG_INFO("[{}_EVOI] {}", name, m_EVOI.GetMean());
    EVOI_list.Append(m_EVOI.GetMean());
#endif
}

void FactorMan::cal_OIR() {
#ifdef CAL_OIR
    // 需要进行数据类型转化，避免整数相除
    m_OIR = static_cast<double>(*(bidv_itr[0]) - *(askv_itr[0])) / (*(bidv_itr[0]) + *(askv_itr[0]));
    LOG_INFO("[{}_OIR] {}", name, m_OIR);
#endif
}
void FactorMan::cal_VTP() {
#ifdef CAL_VTP
    m_VTP = tp * trade_vol;
    LOG_INFO("[{}_VTP] {}", name, m_VTP);
#endif
}
void FactorMan::cal_MPB() {
#ifdef CAL_MPB
    m_MPB = tp - 0.5 * (mp.pre(0) + mp.pre(1));
    MPB_list.Append(m_MPB);
    LOG_INFO("[{}_MPB] {}", name, m_MPB);
#endif
}
void FactorMan::cal_MPBVS() {
#if defined(CAL_MPBVS)
    auto mpbvs = (tp - mid_price) * sqrt(trade_vol * sp * (bidv1 + askv1));
    m_MPBVS.Add(mpbvs);
    MPBVS_list.Append(m_MPBVS.GetMean());
    LOG_INFO("[{}_MPBVS] {}", name, m_MPBVS.GetMean());
#endif
}
void FactorMan::cal_SignVol_1() {
#ifdef CAL_SignVol_1
    auto dp = sqrt(*volume_itr) *
              (0.3 * math_tool::sign(*last_price_itr - *(last_price_itr - 1)) +
               0.5 * math_tool::sign(*TP_itr - *(TP_itr - 1)) + 0.3 * math_tool::sign(*mp_itr - *(mp_itr - 1)));
    m_SignVol_1.Add(dp);
    SignVol_1_list.Append(m_SignVol_1.GetMean());
    LOG_INFO("[{}_TP] {}", this->name, *TP_itr);
    LOG_INFO("[{}_MP] {}", this->name, mid_price);
    LOG_INFO("[{}_SignVol_1] {}", name, m_SignVol_1.GetMean());
#endif
}
void FactorMan::cal_DMB() {
#if defined(CAL_DMB) || defined(CAL_a2f8) || defined(CAL_a1f4)
    m_DMB = (-2 * ask1 * bid1 + (ask1 + bid1) * tp) / (sp * tp);
    LOG_INFO("[{}_DMB] {}", name, m_DMB);
    DMB_list.Append(m_DMB);
#endif
}
void FactorMan::cal_SumSizeDiff() {
#ifdef CAL_SumSizeDiff
    m_sum_bidv = 0;
    m_sum_askv = 0;
    for (int i = 0; i < 5; ++i) {
        m_sum_bidv += bidv[i].pre(0);
        m_sum_askv += askv[i].pre(0);
    }
    m_sumsizediff = log(m_sum_bidv) - log(m_sum_askv);
    SumSizeDiff_list.Append(m_sumsizediff);
    LOG_INFO("[{}_SumSizeDiff] {:.5f}", name, m_sumsizediff);
#endif
}
void FactorMan::cal_ADI() {
#if defined(CAL_ADI) || defined(CAL_L2a1) || defined(CAL_L2a2) || defined(CAL_L2a3) || defined(CAL_L2a4) ||            \
    defined(CAL_a1f1) || defined(CAL_a2f1) || defined(CAL_a2f3)
    m_sum_bidv = 0;
    m_sum_askv = 0;
    for (int i = 0; i < 5; ++i) {
        m_sum_bidv += bidv[i].pre(0);
        m_sum_askv += askv[i].pre(0);
    }
    m_ADI = static_cast<double>(m_sum_bidv - m_sum_askv) / (m_sum_bidv + m_sum_askv);
    ADI_list.Append(m_ADI);
    LOG_INFO1("[{}_ADI] {:.5f}", name.c_str(), m_ADI);
    if (ADI_list.cum_size() >= 5) {
        ADI_rank_5.Append(ff::ts_rank(ADI_list, 5));
        LOG_INFO1("[{}_ADI_rank_5] {:.5f} size {}", name, ADI_rank_5.pre(0), ADI_rank_5.cum_size());
    }
    if (ADI_list.cum_size() >= 6) {
        ADI_rank_6.Append(ff::ts_rank(ADI_list, 6));
        LOG_INFO1("[{}_ADI_rank_6] {:.5f} size {}", name, ADI_rank_6.pre(0), ADI_rank_6.cum_size());
    }
    if (ADI_rank_6.cum_size() >= 80) {
        LOG_INFO1("[{}ADI_rank_6_rank_80] {:.5f}", name, ADI_rank_6_rank_80.pre(0));
        // ADI_rank_6_rank_80.Append(ff::ts_rank(ADI_rank_6, 80));
        LOG_INFO1("[{}ADI_rank_6_rank_80] {:.5f}", name, ADI_rank_6_rank_80.pre(0));
    }
#endif
}
void FactorMan::cal_Dispersion() {
    m_Dispersion =
        0.5 *
            (bidv[0].pre(0) * (bid[0].pre(0) - bid[1].pre(0)) + bidv[1].pre(0) * (bid[1].pre(0) - bid[2].pre(0)) +
             bidv[2].pre(0) * (bid[2].pre(0) - bid[3].pre(0)) + bidv[3].pre(0) * (bid[3].pre(0) - bid[4].pre(0))) /
            (bidv[0].pre(0) + bidv[1].pre(0) + bidv[2].pre(0) + bidv[3].pre(0)) +
        0.5 *
            (askv[0].pre(0) * (ask[1].pre(0) - ask[0].pre(0)) + askv[1].pre(0) * (ask[2].pre(0) - ask[1].pre(0)) +
             askv[2].pre(0) * (ask[3].pre(0) - ask[2].pre(0)) + askv[3].pre(0) * (ask[4].pre(0) - ask[3].pre(0))) /
            (askv[0].pre(0) + askv[1].pre(0) + askv[2].pre(0) + askv[3].pre(0));
    LOG_INFO("[{}_Dispersion] {:.5f}", name, m_Dispersion);  // 检查完，一致
}
void FactorMan::guess_trade_price() {
    double left_amount = this->tp * this->trade_vol;
    auto left_vol = this->trade_vol;
    bid_vol = {0};
    ask_vol = {0};  // cancel_bid{0}, cancel_ask{0};
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

    // Step 1. 假设 last_price成交了1手,
    // i. last_price 小于 mid_price, 定义为bid， ii. last_price 大于等于
    // mid_price，定义为 ask
    if (left_vol > 0) {
        left_vol -= 1;
        // take_lp = 1;
        if (Compare(last_price.pre(0), mid_price) == -1) {
            bid_vol += 1;
            bid_amount += last_price.pre(0);
        } else {
            ask_vol += 1;
            ask_amount += last_price.pre(0);
        }
    } else {
        // DEBUG_GTP;
        return;
    }
    // CHECK_VOL_AMOUNT;

    // Step 2. 假设 盘口扫过的都成交
    for (int i = 0; i < 5; ++i) {
        if ((bid[0].pre(0) < bid[i].pre(1)) && left_vol > 0) {
            // take_bid
            take_bid[i] = bidv[i].pre(1) <= left_vol ? bidv[i].pre(1) : left_vol;
            bid_vol += take_bid[i];
            bid_amount += take_bid[i] * bid[i].pre(1);
            left_vol -= take_bid[i];
        }
        // take ask
        if (ask[0].pre(0) > ask[i].pre(1) && left_vol > 0) {
            take_ask[i] = askv[i].pre(1) <= left_vol ? askv[i].pre(1) : left_vol;
            ask_vol += take_ask[i];
            ask_amount += take_ask[i] * ask[i].pre(1);
            left_vol -= take_ask[i];
        }
    }
    left_amount -= ask_amount + bid_amount;
    // CHECK_VOL_AMOUNT;
    if (Compare(fabs(left_amount), 0) == 0) {
        m_VPI = static_cast<double>(ask_vol - bid_vol) / trade_vol;
        m_PID = (ask_amount - bid_amount) / (bid_amount + ask_amount);
        // DEBUG_GTP;
        return;
    }

    // Step 3. 将剩余的成交量分到 bid1, ask1
    ab_vol = left_vol;
    if (ab_vol > 0) {
        if (ask[0].pre(0) * ab_vol - left_amount < 0) {
            // all on ask
            take_a1 = ab_vol;
        } else if (left_amount - bid[0].pre(0) * ab_vol < 0) {
            // all on bid
            take_b1 = ab_vol;
        } else {
            // both on ask and bid
            take_a1 = hft::Round((left_amount - bid[0].pre(0) * ab_vol) / this->sp);
            take_b1 = hft::Round((ask[0].pre(0) * ab_vol - left_amount) / this->sp);
        }
        left_amount -= take_a1 * ask[0].pre(0) + take_b1 * bid[0].pre(0);
        left_vol -= take_b1 + take_a1;
        ask_amount += take_a1 * ask[0].pre(0);
        bid_amount += take_b1 * bid[0].pre(0);
        ask_vol += take_a1;
        bid_vol += take_b1;
    }
    // CHECK_VOL_AMOUNT;
    if (Compare(fabs(left_amount), 0) == 0) {
        m_VPI = static_cast<double>(ask_vol - bid_vol) / trade_vol;
        m_PID = (ask_amount - bid_amount) / (bid_amount + ask_amount);
        // DEBUG_GTP;
        return;
    }

    // Step 4. 将多余的一手就近分到ask1或bid1上
    // near ask
    if (left_vol != 0) {
        if (((left_vol == 1 && ask[0].pre(0) - left_amount < left_amount - bid[0].pre(0) + 0.01) ||
             (left_vol == -1 && ask[0].pre(0) + left_amount < -left_amount - bid[0].pre(0) + 0.01)) &&
            left_vol + take_a1 >= 0) {
            take_a1 += left_vol;
            ask_vol += left_vol;
            left_amount -= left_vol * ask[0].pre(0);
            ask_amount += ask[0].pre(0) * left_vol;
            left_vol = 0;
        } else {
            // near bid
            take_b1 += left_vol;
            bid_vol += left_vol;
            bid_amount += bid[0].pre(0) * left_vol;
            left_amount -= left_vol * bid[0].pre(0);
            left_vol = 0;
        }
    }
    // CHECK_VOL_AMOUNT;

    if (Compare(fabs(left_amount), 0) == 0) {
        m_VPI = static_cast<double>(ask_vol - bid_vol) / trade_vol;
        m_PID = (ask_amount - bid_amount) / (bid_amount + ask_amount);
        // DEBUG_GTP;
        return;
    }
    // Step 5. 将多余的成交额分到bid1, ask1...
    if (take_b1 > 0) {
        take_b1 -= 1;
        gp1 = bid[0].pre(0) + left_amount;
        gvp1 = 1;
        left_amount -= left_amount;
        bid_vol -= 1;
        bid_amount -= bid[0].pre(0);
    } else if (take_a1 > 0) {
        take_a1 -= 1;
        gp1 = ask[0].pre(0) + left_amount;
        gvp1 = 1;
        left_amount -= left_amount;
        ask_vol -= 1;
        ask_amount -= ask[0].pre(0);
    } else {
        for (int i = 0; i < 5; ++i) {
            if (take_bid[i] > 0 && Compare(left_amount, 0) != 0) {
                take_bid[i] -= 1;
                bid_vol -= 1;
                bid_amount -= bid[i].pre(1);
                gvp1 = 1;
                gp1 = bid[i].pre(1) + left_amount;
                left_amount -= left_amount;
            }
        }
        for (int i = 0; i < 5; ++i) {
            if (take_ask[i] > 0 && Compare(left_amount, 0) != 0) {
                take_ask[i] -= 1;
                ask_amount -= ask[i].pre(1);
                ask_vol -= 1;
                gvp1 = 1;
                gp1 = ask[i].pre(1) + left_amount;
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
    // CHECK_VOL_AMOUNT;
    m_VPI = static_cast<double>(ask_vol - bid_vol) / trade_vol;
    m_PID = (ask_amount - bid_amount) / (bid_amount + ask_amount);
    // DEBUG_GTP;
}
void FactorMan::cal_AsyBidAsk() {
    LOG_INFO1("[{}_ask1] {}", name.c_str(), ask1);           // 检查完，一致
    LOG_INFO1("[{}_ask5] {}", name.c_str(), ask[4].pre(0));  // 检查完，一致
    LOG_INFO1("[{}_bid1] {}", name.c_str(), bid1);           // 检查完，一致
    LOG_INFO1("[{}_bid5] {}", name.c_str(), bid[4].pre(0));  // 检查完，一致
    m_AsyBidAsk = log((ask[4].pre(0) - ask[0].pre(0)) / (bid[0].pre(0) - bid[4].pre(0)));
    sab = math_tool::sign(m_AsyBidAsk);
    AsyBidAsk_list.Append(m_AsyBidAsk);
    LOG_INFO1("[{}_AsyBidAsk] {:.5f}", name.c_str(), m_AsyBidAsk);  // 检查完，一致
}
void FactorMan::cal_ABvol() {
    LOG_INFO1("[{}_last_price] {}", this->name.c_str(), last_price.pre(0));
    guess_trade_price();
    ask_vol_list.Append(ask_vol);
    bid_vol_list.Append(bid_vol);
    ask_amount_list.Append(ask_amount);
    bid_amount_list.Append(bid_amount);
    VPI_list.Append(m_VPI);
    LOG_INFO("[{}_PID] {}", this->name, m_PID);
    LOG_INFO("[{}_VPI] {}", this->name, m_VPI);
    LOG_INFO1("[{}_ask_vol] {}", this->name, ask_vol);
    LOG_INFO1("[{}_bid_vol] {}", this->name, bid_vol);
    LOG_INFO("[{}_bid_amount] {}", this->name, bid_amount);
    LOG_INFO("[{}_ask_amount] {}", this->name, ask_amount);
}
void FactorMan::cal_L2f2() {
    //  df['AsyBidAsk'] = np.log((tk.ask5 - tk.ask1) / (tk.bid1 - tk.bid5))
    auto s1 = math_tool::sign(tp - tp_list.pre(1));
    auto sab = math_tool::sign(ask_vol - bid_vol);
    auto stp = math_tool::sign(tp - mid_price);
    if (fabs(m_AsyBidAsk) <= 0.5) {
        L2f2 = m_AsyBidAsk;
        return;
    } else if (m_AsyBidAsk * (s1 + sab + stp) > 0) {
        L2f2 = m_AsyBidAsk * 2.5;
        return;
    } else if (m_AsyBidAsk * s1 < 0 && m_AsyBidAsk * sab < 0 && m_AsyBidAsk * stp < 0) {
        L2f2 = m_AsyBidAsk * 0.2;
        return;
    } else {
        L2f2 = m_AsyBidAsk;
        return;
    }
}
};  // namespace hft
