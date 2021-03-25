#pragma once

//#define HFT_DEBUG
//#define LOG_LEVEL_1

#include "alpha/alpha.h"
#include "hft/FactorMan.h"
#include "hft/FuturesPairInfo.hpp"
#include "hft/MarketData.h"
#include "hft/futures_pair_info.hpp"
#define TimeStemp0 1200000000
using namespace std;    // NOLINT
using namespace husky;  // NOLINT
// static size_t counter{0};
void FillFPFactor(const hft::FactorMan& IC1, const hft::FactorMan& IC2, const hft::FactorMan& IH1,
                  const hft::FactorMan& IF1, hft::fpfactor* output);
uint32_t GetTimeStemp(const md::l2& tick) {
    auto tmp = 2 * (tick.update_time - TimeStemp0) + tick.update_milli_sec / 500;
    return tmp;
}
class Strapro : public alpha {
   private:
    uint32_t t_IC1;
    uint32_t t_IC2;
    uint32_t t_IH1;
    uint32_t t_IF1;
    bool do_init;

   public:
    Strapro();
    // 自定义的策略名，只用来显示
    std::string name() override { return "alpha_IC_demo_v1.0"; }

    // 盘后变量重置
    void do_finalize() override;

    // tick更新之后，会调用此方法。（信号计算）
    void calc(uint /*di*/) override;

   private:
    bool m_fillFv, use_IC1, use_IC2, use_IF1, use_IF2, use_IH1, use_IH2;
    uint32_t min_ticks;
    uint32_t init_ticks;
    int32_t Ntick;
    int32_t target_pos;
    int32_t m_orders;
    std::string data_path, trigger;
    std::string m_order_book;

    basic_matrix<md::l2>* trigger_tick;
    basic_matrix<md::l2> IC1;
    basic_matrix<md::l2> IC2;
    basic_matrix<md::l2> IH1;
    basic_matrix<md::l2> IH2;
    basic_matrix<md::l2> IF1;
    basic_matrix<md::l2> IF2;

    hft::FactorMan* trigger_data;
    hft::FactorMan IC1_data;
    hft::FactorMan IC2_data;
    hft::FactorMan IF1_data;
    hft::FactorMan IF2_data;
    hft::FactorMan IH1_data;
    hft::FactorMan IH2_data;
    // hft::FactorMan factor_man;
    hft::fpfactor fp_factor;
    double m_threshold;
    // 下单策略
    void TradeOrder();
    // 卖出指定手数
    void NewSellOrder(uint32_t size, uint32_t trade_mode);
    // 买入指定手数
    void NewBuyOrder(uint32_t size, uint32_t trade_mode);
    void init_data();
    void re_set_data();
};

Strapro::Strapro()
    : alpha(),
      do_init{true},
      use_IC1{false},
      use_IC2{false},
      use_IF1{false},
      use_IF2{false},
      use_IH1{false},
      use_IH2{false},
      min_ticks{125},
      init_ticks{125},
      trigger_data{nullptr},
      IC1_data{"IC1", min_ticks},
      IC2_data{"IC2", min_ticks},
      IF1_data{"IF1", min_ticks},
      IF2_data{"IF2", min_ticks},
      IH1_data{"IH1", min_ticks},
      IH2_data{"IH2", min_ticks}  // factor_man{"IC1", min_ticks},
{
    this->init_data();
    // dr_load("IC1", &IC1, husky::CFFEX);
    // dr_load("IC2", &IC2, husky::CFFEX);
    // dr_load("IF1", &IF1, husky::CFFEX);
    // dr_load("IH1", &IH1, husky::CFFEX);
    // factor_man.SetDoCal(true);
    auto log_path = ALPHA_CONFIG->get_string("sim.log_path", "");
    printf("%s \n", log_path.c_str());
    m_order_book = log_path + "/orderbook.csv";
    FILE* fp = fopen(m_order_book.c_str(), "w");
    fprintf(fp, "time,us,order_direct,size\n");
    fclose(fp);

#ifdef HFT_DEBUG
    HUSKY_TRADE_INFO("the 'HFT_DEBUG' is defined");
#else
    HUSKY_TRADE_INFO("the 'HFT_DEBUG' is not defined");
#endif
}
void PrintFactorMan(const hft::FactorMan& factor_man, const md::l2& tick) {
    LOG_INFO("[FACTOR] "
             "{},{},{},{},{},{},"
             "{},{},{},{},"
             "{},{},{},{},"
             "{},{},{},{},"
             "{},{},{},{},"
             "{},{},{},{},"
             "{:.5f},{:.5f},"  // tp, sp
             "{:.5f},{:.5f},{:.5f},{:.5f}",
             tick.update_time, tick.update_milli_sec,  // time
             factor_man.last_price.pre(0), factor_man.lastamount.pre(0), factor_man.lastopenint.pre(0),
             factor_man.trade_vol,  // ticks
             factor_man.bid[0].pre(0), factor_man.bidv[0].pre(0), factor_man.ask[0].pre(0),
             factor_man.askv[0].pre(0),                             // 1
             tick.bid[1], tick.bidv[1], tick.ask[1], tick.askv[1],  // 2
             tick.bid[2], tick.bidv[2], tick.ask[2], tick.askv[2],  // 3
             tick.bid[3], tick.bidv[3], tick.ask[3], tick.askv[3],  // 4
             tick.bid[4], tick.bidv[4], tick.ask[4], tick.askv[4],  // 5
             factor_man.tp, factor_man.sp,                          // tp, sp
             // factor_man.VOI(), factor_man.EVOI(), factor_man.OIR(), factor_man.VTP(),
             // factor_man.DMB()  // factor 1
             factor_man.AsyBidAsk(), factor_man.SumSizeDiff(), factor_man.ADI(), factor_man.Dispersion());
}
void CheckNan(const hft::FactorMan& factor_man) {
    if (std::isnan(factor_man.MPBVS())) {
        LOG_INFO("[CheckNan] MPBVS is {} tp {} mid_price {} trade_vol {}, sp {} bidv1 {}, askv1 {}", factor_man.MPBVS(),
                 factor_man.tp, factor_man.mid_price, factor_man.trade_vol, factor_man.sp, factor_man.bidv1,
                 factor_man.askv1);
    }
    if (std::isnan(factor_man.tp)) {
        LOG_INFO("[CheckNan] tp is {}", factor_man.tp);
    }
    if (std::isnan(factor_man.sp)) {
        LOG_INFO("[CheckNan] sp is {}", factor_man.sp);
    }
    if (std::isnan(factor_man.mid_price)) {
        LOG_INFO("[CheckNan] mid_price is {}", factor_man.mid_price);
    }
    if (std::isnan(factor_man.trade_vol)) {
        LOG_INFO("[CheckNan] trade_vol is {}", factor_man.trade_vol);
    }
}

extern "C" alpha* create_instance() { return new Strapro(); }
void FillFPFactor(const hft::FactorMan& IC1, const hft::FactorMan& IC2, const hft::FactorMan& IH1,
                  const hft::FactorMan& IF1, hft::fpfactor* output) {
    hft::fpBase1(IC1, IC2, 14, &(output->fpf1));
    LOG_INFO("[fpf1] {}", output->fpf1);
    hft::fpBase1(IC1, IH1, 14, &(output->fpf2));
    LOG_INFO("[fpf2] {}", output->fpf2);
    hft::fpBase1(IC1, IF1, 14, &(output->fpf3));
    LOG_INFO("[fpf3] {}", output->fpf3);
    hft::fpBase1(IC1, IC2, 40, &(output->fpf4));
    hft::fpBase1(IC1, IH1, 40, &(output->fpf5));
    hft::fpBase1(IC1, IF1, 40, &(output->fpf6));
    hft::fpBase1(IC1, IC2, 80, &(output->fpf7));
    hft::fpBase1(IC1, IH1, 80, &(output->fpf8));
    hft::fpBase1(IC1, IF1, 80, &(output->fpf9));

    if (IC1.TickeLength() == 1) {
        hft::fpBase2(IC1, IC2, 1, &(output->fpf10));
        hft::fpBase2(IC1, IH1, 1, &(output->fpf11));
        hft::fpBase2(IC1, IF1, 1, &(output->fpf12));
    } else {
        hft::fpBase2(IC1, IC2, 10, &(output->fpf10));
        hft::fpBase2(IC1, IH1, 10, &(output->fpf11));
        hft::fpBase2(IC1, IF1, 10, &(output->fpf12));
    }
    hft::fpBase3(IC1, IF1, 10, &(output->fpf13));
    hft::fpBase3(IC1, IF1, 40, &(output->fpf14));
    hft::fpBase3(IC1, IC2, 10, &(output->fpf15));
    LOG_INFO("[fpf15] {}", output->fpf15);
    hft::fpBase3(IC1, IC2, 40, &(output->fpf16));
    hft::fpBase3(IC1, IH1, 10, &(output->fpf17));
    hft::fpBase3(IC1, IH1, 40, &(output->fpf18));
    LOG_INFO("[fpf4] {}", output->fpf4);
    LOG_INFO("[fpf5] {}", output->fpf5);
    LOG_INFO("[fpf6] {}", output->fpf6);
    LOG_INFO("[fpf7] {}", output->fpf7);
    LOG_INFO("[fpf8] {}", output->fpf8);
    LOG_INFO("[fpf9] {}", output->fpf9);
    LOG_INFO("[fpf10] {}", output->fpf10);
    LOG_INFO("[fpf11] {}", output->fpf11);
    LOG_INFO("[fpf12] {}", output->fpf12);
    LOG_INFO("[fpf13] {}", output->fpf13);
    LOG_INFO("[fpf14] {}", output->fpf14);
    LOG_INFO("[fpf16] {}", output->fpf16);
    LOG_INFO("[fpf17] {}", output->fpf17);
    LOG_INFO("[fpf18] {}", output->fpf18);
}
