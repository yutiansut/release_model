#include "mydefine.h"

#define LOG_LEVEL_5

#include "BDT_model.hpp"
#include "StraPro.hpp"
#include "alpha/alpha.h"
#include "bench_mark.hpp"
#include "hft/FactorMan.h"
#include "hft/FuturesPairInfo.hpp"
#include "hft/MarketData.h"
#include "hft/base.h"
using namespace std;    // NOLINT
using namespace husky;  // NOLINT

static size_t counter{0};
static int32_t trade_mode;
static int32_t max_pos;
static std::vector<uint32_t> dur_list;
void Strapro::init_data() {
    auto trade_contract = ALPHA_CONFIG->get_string("trade_contract", "IC1");
    printf("The trade contract is %s\n", trade_contract.c_str());
    trigger = ALPHA_CONFIG->get_string("triggers", "IC1");
    printf("trigger is %s\n", trigger.c_str());
    // get deps
    std::string deps = ALPHA_CONFIG->get_string("deps", "");
    std::vector<std::string> deps_pro;
    hft::Split(deps, &deps_pro);
    for (auto i : deps_pro) {
        printf("use: %s\n", i.c_str());
    }
    if (std::find(deps_pro.begin(), deps_pro.end(), "IC1") != deps_pro.end() || trigger == "IC1") {
        use_IC1 = true;
        dr_load("IC1", &IC1, husky::CFFEX);
        printf("dr_load IC1 success \n");
    }
    if (std::find(deps_pro.begin(), deps_pro.end(), "IC2") != deps_pro.end() || trigger == "IC2") {
        use_IC2 = true;
        dr_load("IC2", &IC2, husky::CFFEX);
        printf("dr_load IC2 success \n");
    }
    if (std::find(deps_pro.begin(), deps_pro.end(), "IF1") != deps_pro.end() || trigger == "IF1") {
        use_IF1 = true;
        dr_load("IF1", &IF1, husky::CFFEX);
        printf("dr_load IF1 success \n");
    }
    if (std::find(deps_pro.begin(), deps_pro.end(), "IF2") != deps_pro.end() || trigger == "IF2") {
        use_IF2 = true;
        dr_load("IF2", &IF2, husky::CFFEX);
        printf("dr_load IF2 success \n");
    }
    if (std::find(deps_pro.begin(), deps_pro.end(), "IH1") != deps_pro.end() || trigger == "IH1") {
        use_IH1 = true;
        dr_load("IH1", &IH1, husky::CFFEX);
        printf("dr_load IH1 success \n");
    }
    if (std::find(deps_pro.begin(), deps_pro.end(), "IH2") != deps_pro.end() || trigger == "IH2") {
        use_IH2 = true;
        dr_load("IH2", &IH2, husky::CFFEX);
        printf("dr_load IH2 success \n");
    }
    // printf("%s,%d \n", __FUNCTION__, __LINE__);
    if (trigger == "IC1") {
        trigger_data = &IC1_data;
        trigger_tick = &IC1;
        IC1_data.SetDoCal(true);
    }
    // printf("%s,%d \n", __FUNCTION__, __LINE__);
    if (trigger == "IC2") {
        trigger_data = &IC2_data;
        trigger_tick = &IC2;
        IC2_data.SetDoCal(true);
    }
    if (trigger == "IF1") {
        trigger_data = &IF1_data;
        trigger_tick = &IF1;
        IF1_data.SetDoCal(true);
    }
    if (trigger == "IF2") {
        trigger_data = &IF2_data;
        trigger_tick = &IF2;
        IF2_data.SetDoCal(true);
    }
    if (trigger == "IH1") {
        trigger_data = &IH1_data;
        trigger_tick = &IH1;
        IH1_data.SetDoCal(true);
    }
    if (trigger == "IH2") {
        trigger_data = &IH2_data;
        trigger_tick = &IH2;
        IH2_data.SetDoCal(true);
    }
    trigger_data->SetDoCal(true);
    //
    Ff7_caler.set_period(10);
    Ff7_10_caler.set_period(10);
    Ff7_20_caler.set_period(20);
    Ff7_40_caler.set_period(40);
    Ff7_80_caler.set_period(80);
    Ff8_2_caler.set_period(40);
    L2f10_10_caler.set_period(10);
    L2f10_20_caler.set_period(20);
    L2f10_40_caler.set_period(40);
    L2f10_60_caler.set_period(60);
    L2f10_80_caler.set_period(80);
}
void Strapro::re_set_data() {
    if (use_IC1) IC1_data.ReSet();
    if (use_IC2) IC2_data.ReSet();
    if (use_IF1) IF1_data.ReSet();
    if (use_IF2) IF2_data.ReSet();
    if (use_IH1) IH1_data.ReSet();
    if (use_IH2) IH2_data.ReSet();
    IC1_factor_data.ReSet();
}
void Strapro::do_finalize() {
    // printf("%s %d\n", __FUNCTION__, __LINE__);
    std::string trade_date = hft::TimesecToString(trigger_tick->operator()(0).update_time, "%Y-%m-%d");
    LOG_INFO1(">>>>>> process {} <<<<<<", trade_date);
    this->re_set_data();
    counter = 0;
    // factor_man.ReSet();
    // const auto &tick = trigger_tick->operator()(0);
    // t_IC1 = GetTimeStemp(tick);
    // LOG_INFO("time:{}", t_IC1);
    do_init = true;
    m_threshold = ALPHA_CONFIG->get_double("threshold", 1.2);
    Ntick = ALPHA_CONFIG->get_int("Ntick", 3);
    trade_mode = ALPHA_CONFIG->get_int("trade_mode", 1);
    // printf("trade_mode %d\n", trade_mode);
    max_pos = ALPHA_CONFIG->get_int("max_pos", 1);
    // printf("max_pos %d\n", max_pos);
    int first_tick{1};
    md::l2 *tick_list = new md::l2[init_ticks];
    for (uint32_t i = 0; i < init_ticks; ++i) {
        int cur_tick = first_tick + i;
        tick_list[i] = IC1(cur_tick);
        if (use_IC1) IC1_data.AppendTick(IC1(cur_tick));
        if (use_IC2) IC2_data.AppendTick(IC2(cur_tick));
        if (use_IF1) IF1_data.AppendTick(IF1(cur_tick));
        if (use_IF2) IF2_data.AppendTick(IF2(cur_tick));
        if (use_IH1) IH1_data.AppendTick(IH1(cur_tick));
        if (use_IH2) IH2_data.AppendTick(IH2(cur_tick));
        ++counter;
    }
    IC1_factor_data.init_data(tick_list, init_ticks);
    Ff7_caler.init_data(&IC1_factor_data);
    Ff7_10_caler.init_data(&IC1_factor_data);
    Ff7_20_caler.init_data(&IC1_factor_data);
    Ff7_40_caler.init_data(&IC1_factor_data);
    Ff7_80_caler.init_data(&IC1_factor_data);
    Ff8_2_caler.init_data(&IC1_factor_data);
    L2a1_caler.init_data(&IC1_factor_data);
    L2a2_caler.init_data(&IC1_factor_data);
    L2f10_10_caler.init_data(&IC1_factor_data);
    L2f10_20_caler.init_data(&IC1_factor_data);
    L2f10_40_caler.init_data(&IC1_factor_data);
    L2f10_60_caler.init_data(&IC1_factor_data);
    L2f10_80_caler.init_data(&IC1_factor_data);
    L2f9_caler.init_data(&IC1_factor_data);
    L2p1_caler.init_data(&IC1_factor_data);
    L2p2_caler.init_data(&IC1_factor_data);
    VOI_caler.init_data(&IC1_factor_data);
    a1f10_caler.init_data(&IC1_factor_data);
    a1f11_caler.init_data(&IC1_factor_data);
    rank1_caler.init_data(&IC1_factor_data);
    rank2_caler.init_data(&IC1_factor_data);
}

void Strapro::NewSellOrder(uint32_t size, uint32_t trade_mode) {
    auto net_pos = this->get_net_position();
    auto sell_price = trigger_tick->operator()(0).bid[0];
    if (trade_mode > 0) {
        sell_price = MAX(trigger_tick->operator()(0).bid[0], trigger_tick->operator()(0).ask[0] - 0.2 * trade_mode);
    }
    LOG_INFO1("[{}] sell_price: {:.2f}", __FUNCTION__, sell_price);
    auto target_pos = net_pos - size;
    if (target_pos > 0) {
        insert(sell_price, oms::SideBuy, target_pos);
    } else if (target_pos < 0) {
        insert(sell_price, oms::SideSell, -target_pos);
    }
    LOG_INFO1("[{}] save to order book", __FUNCTION__);
    FILE *fp = fopen(m_order_book.c_str(), "a");
    fprintf(fp, "%u,%u,%s,%d\n", trigger_tick->operator()(0).update_time, trigger_tick->operator()(0).update_milli_sec,
            "sell", size);
    fclose(fp);
}
void Strapro::NewBuyOrder(uint32_t size, uint32_t trade_mode) {
    auto buy_price = trigger_tick->operator()(0).ask[0];
    if (trade_mode > 0) {
        buy_price = MIN(trigger_tick->operator()(0).ask[0], trigger_tick->operator()(0).bid[0] + 0.2 * trade_mode);
    }
    LOG_INFO1("[{}] buy_price: {:.2f}", __FUNCTION__, buy_price);
    auto net_pos = this->get_net_position();
    auto target_pos = net_pos + size;
    if (target_pos > 0) {
        insert(buy_price, oms::SideBuy, target_pos);
        LOG_INFO1("[{}] insert({:.2f}, SideBuy, {})", __FUNCTION__, buy_price, target_pos);
    } else if (target_pos < 0) {
        insert(buy_price, oms::SideSell, -target_pos);
        LOG_INFO1("[{}] insert({:.2f}, SideSell, {})", __FUNCTION__, buy_price, -target_pos);
    }
    LOG_INFO1("[{}] save to order book", __FUNCTION__);
    FILE *fp = fopen(m_order_book.c_str(), "a");
    fprintf(fp, "%u,%u,%s,%d\n", trigger_tick->operator()(0).update_time, trigger_tick->operator()(0).update_milli_sec,
            "buy", size);
    fclose(fp);
}

void Strapro::TradeOrder() {
    if (m_orders <= 0 || this->get_net_position() == this->target_pos) {
        return;
    }
    // LOG_INFO("NetPositions:{} target_pos:{} sp:{:.2f}",
    // get_net_position(), target_pos, factor_man.sp);
    if (target_pos > 0) {
        // cancel_all();
        auto ask1 = trigger_tick->operator()(0).ask[0];
        auto bid1 = trigger_tick->operator()(0).bid[0];
        auto buy_price = ask1;
        if (trade_mode > 0) {
            buy_price = MIN(ask1, bid1 + 0.2 * trade_mode);
        }
        // NewOrder<OrderDirect::kSell>(buy_price, target_pos);
        LOG_INFO("BUY @ {}", buy_price);
        insert(buy_price, oms::SideBuy, target_pos);
        FILE *fp = fopen(m_order_book.c_str(), "a");
        fprintf(fp, "%u,%u,%s,%d\n", trigger_tick->operator()(0).update_time,
                trigger_tick->operator()(0).update_milli_sec, "buy", target_pos);
        fclose(fp);
        m_orders -= 1;
    } else if (target_pos < 0) {
        // cancel_all();
        m_orders -= 1;
        auto ask1 = trigger_tick->operator()(0).ask[0];
        auto bid1 = trigger_tick->operator()(0).bid[0];
        auto sell_price = bid1;
        if (trade_mode > 0) {
            sell_price = MAX(bid1, ask1 - 0.2 * trade_mode);
        }
        // NewOrder<OrderDirect::kBuy>(sell_price, target_pos);
        LOG_INFO("SELL @ {}", sell_price);
        insert(sell_price, oms::SideSell, -target_pos);
        FILE *fp = fopen(m_order_book.c_str(), "a");
        fprintf(fp, "%u,%u,%s,%d\n", trigger_tick->operator()(0).update_time,
                trigger_tick->operator()(0).update_milli_sec, "sell", target_pos);
        fclose(fp);
    }
}

void Strapro::calc(uint) {
    // LOG_INFO5("===================={}==============", ++counter);
    auto tag_start = NOW();
    if (trigger_tick->operator()(10).update_time == 0) return;
    cancel_all();

    LOG_INFO2("[{}] updatetime {} | stemp: {}", __FUNCTION__,
              hft::TimesecToString(trigger_tick->operator()(0).update_time).c_str(),
              trigger_tick->operator()(0).update_time);
    if (trigger_tick->operator()(10).update_time == 0) return;
    // LOG_INFO("[update_time] {} {} {} {}", t_IC1, t_IC2, t_IF1, t_IH1);
    LOG_INFO1("ask1 IC1 {}, update_time:{}", IC1(0).ask[0], IC1(0).update_time);
    if (trigger == "IC2") {
        LOG_INFO1("IC2: ask {} {} {} {} {}", IC2(0).ask[0], IC2(0).ask[1], IC2(0).ask[2], IC2(0).ask[3], IC2(0).ask[4]);
        LOG_INFO1("IC2: bid {} {} {} {} {}", IC2(0).bid[0], IC2(0).bid[1], IC2(0).bid[2], IC2(0).bid[3], IC2(0).bid[4]);
    }

    if (use_IC1) {
        LOG_INFO1("[{}] deal with IC1", __FUNCTION__);
        IC1_data.AppendTick(IC1(0));
        LOG_INFO1("[{}] deal with IC1", __FUNCTION__);
    }
    if (use_IC2) {
        LOG_INFO1("[{}] deal with IC2", __FUNCTION__);
        IC2_data.AppendTick(IC2(0));
    }
    if (use_IF1) IF1_data.AppendTick(IF1(0));
    if (use_IF2) IF2_data.AppendTick(IF2(0));
    if (use_IH1) IH1_data.AppendTick(IH1(0));
    if (use_IH2) IH2_data.AppendTick(IH2(0));
    // LOG_INFO1("ask1 IC2 {}", IC2(0).ask[0]);
    // LOG_INFO1("time IC1 {}", IC2(0).update_time);

    //#include "use_factor.h"
    // FillFPFactor(factor_man, IC2_data, IH1_data, IF1_data, &fp_factor);
    if (use_IC1) {
#ifdef CAL_FPC1_IC1
        FPC1_IC1 = hft::factor::FPC1(trigger_data, &IC1_data, 20);
        LOG_INFO2("FPC1_IC1 {}", FPC1_IC1);
#endif
#ifdef CAL_FPC2_IC1
        FPC2_IC1 = hft::factor::FPC2(trigger_data, &IC1_data, 20);
        LOG_INFO2("FP2_IC1 {}", FPC2_IC1);
#endif
#ifdef CAL_FPC3_IC1
        FPC3_IC1 = hft::factor::FPC3(trigger_data, &IC1_data, 20);
        LOG_INFO2("[{}] FPC3_IC1:{}", __FUNCTION__, FPC3_IC1);
#endif
#ifdef CAL_FPC4_IC1
        FPC4_IC1 = hft::factor::FPC4(trigger_data, &IC1_data, 20);
        LOG_INFO2("FPC4_IC1 {}", FPC4_IC1);
#endif
#ifdef CAL_FPD1_IC1
        FPD1_IC1 = hft::factor::FPD1(trigger_data, &IC1_data, 20);
        LOG_INFO2("FPD1_IC1 {}", FPD1_IC1);
#endif
#ifdef CAL_FPD2_IC1
        FPD2_IC1 = hft::factor::FPD2(trigger_data, &IC1_data, 20);
        LOG_INFO2("FPD2_IC1 {}", FPD2_IC1);
#endif
#ifdef CAL_FPD3_IC1
        FPD3_IC1 = hft::factor::FPD3(trigger_data, &IC1_data);
        LOG_INFO2("FPD3_IC1 {}", FPD3_IC1);
#endif
#ifdef CAL_FPD4_IC1
        FPD4_IC1 = hft::factor::FPD4(trigger_data, &IC1_data, 20);
        LOG_INFO2("FPD4_IC1 {}", FPD4_IC1);
#endif
    }
    if (use_IF1) {

#ifdef CAL_FPC1_IF1
        FPC1_IF1 = hft::factor::FPC1(trigger_data, &IF1_data, 20);
#endif
#ifdef CAL_FPC2_IF1
        FPC2_IF1 = hft::factor::FPC2(trigger_data, &IF1_data, 20);
#endif
#ifdef CAL_FPC3_IF1
        FPC3_IF1 = hft::factor::FPC3(trigger_data, &IF1_data, 20);
#endif
#ifdef CAL_FPC4_IF1
        FPC4_IF1 = hft::factor::FPC4(trigger_data, &IF1_data, 20);
#endif
#ifdef CAL_FPD1_IF1
        FPD1_IF1 = hft::factor::FPD1(trigger_data, &IF1_data, 20);
#endif
#ifdef CAL_FPD2_IF1
        FPD2_IF1 = hft::factor::FPD2(trigger_data, &IF1_data, 20);
#endif
#ifdef CAL_FPD3_IF1
        FPD3_IF1 = hft::factor::FPD3(trigger_data, &IF1_data);
#endif
#ifdef CAL_FPD4_IF1
        FPD4_IF1 = hft::factor::FPD4(trigger_data, &IF1_data, 20);
#endif
    }
    if (use_IH1) {
#ifdef CAL_FPC1_IH1
        FPC1_IH1 = hft::factor::FPC1(trigger_data, &IH1_data, 20);
#endif
#ifdef CAL_FPC2_IH1
        FPC2_IH1 = hft::factor::FPC2(trigger_data, &IH1_data, 20);
#endif
#ifdef CAL_FPC3_IH1
        FPC3_IH1 = hft::factor::FPC3(trigger_data, &IH1_data, 20);
#endif
#ifdef CAL_FPC4_IH1
        FPC4_IH1 = hft::factor::FPC4(trigger_data, &IH1_data, 20);
#endif
#ifdef CAL_FPD1_IH1
        FPD1_IH1 = hft::factor::FPD1(trigger_data, &IH1_data, 20);
#endif
#ifdef CAL_FPD2_IH1
        FPD2_IH1 = hft::factor::FPD2(trigger_data, &IH1_data, 20);
#endif
#ifdef CAL_FPD3_IH1
        FPD3_IH1 = hft::factor::FPD3(trigger_data, &IH1_data);
#endif
#ifdef CAL_FPD4_IH1
        FPD4_IH1 = hft::factor::FPD4(trigger_data, &IH1_data, 20);
#endif
    }

    IC1_factor_data.eval(IC1(0));
    // double tmp_Ff7 = Ff7_caler.eval(&IC1_factor_data);
    float new_Ff7_10 = Ff7_10_caler.eval(&IC1_factor_data);
    float new_Ff7_20 = Ff7_20_caler.eval(&IC1_factor_data);
    float new_Ff7_40 = Ff7_40_caler.eval(&IC1_factor_data);
    float new_Ff7_80 = Ff7_80_caler.eval(&IC1_factor_data);
    float new_Ff8_2 = Ff8_2_caler.eval(&IC1_factor_data);
    float new_L2f8 = L2f8_caler.eval(&IC1_factor_data);
    float new_L2f1 = IC1_factor_data.L2f1;
    float new_L2a1 = L2a1_caler.eval(&IC1_factor_data);
    float new_L2a2 = L2a2_caler.eval(&IC1_factor_data);  // 20ns
    float new_L2f6 = L2f6_caler.eval(&IC1_factor_data);
    float new_L2f10_10 = L2f10_10_caler.eval(&IC1_factor_data);
    float new_L2f10_20 = L2f10_20_caler.eval(&IC1_factor_data);
    float new_L2f10_40 = L2f10_40_caler.eval(&IC1_factor_data);
    float new_L2f10_60 = L2f10_60_caler.eval(&IC1_factor_data);
    float new_L2f10_80 = L2f10_80_caler.eval(&IC1_factor_data);
    float new_L2f10 = 0.5 * (new_L2f10_10 + new_L2f10_20 + new_L2f10_40 + new_L2f10_80);
    float new_L2f9 = L2f9_caler.eval(&IC1_factor_data);
    float new_L2p1 = L2p1_caler.eval(&IC1_factor_data);
    float new_L2p2 = L2p2_caler.eval(&IC1_factor_data);
    float new_VOI = IC1_factor_data.VOI;
    // double new_VOI = VOI_caler.eval(&IC1_factor_data);
    // double new_a1f10 = a1f10_caler.eval(&IC1_factor_data);
    // double new_a1f11 = a1f11_caler.eval(&IC1_factor_data);
    float new_a1f10 = IC1_factor_data.a1f10();
    float new_a1f11 = IC1_factor_data.a1f11();
    float new_a1f12 = IC1_factor_data.a1f12();
    float new_a1f13 = IC1_factor_data.a1f13();
    float new_a1f14 = IC1_factor_data.a1f14();
    float new_a1f15 = IC1_factor_data.a1f15();
    float new_a1f4 = IC1_factor_data.a1f4();
    float new_a1f5 = IC1_factor_data.a1f5();
    float new_a1f6 = IC1_factor_data.a1f6();
    float new_a1f7 = IC1_factor_data.a1f7();
    float new_a1f9 = IC1_factor_data.a1f9();

    float new_a2f5 = IC1_factor_data.a2f5();
    float new_a2f6 = IC1_factor_data.a2f6();
    float new_a2f7 = IC1_factor_data.a2f7();
    float new_a2f12 = IC1_factor_data.a2f12();
    float new_a2f13 = IC1_factor_data.a2f13();
    float new_sa2 = IC1_factor_data.sa2();
    float new_sa3 = IC1_factor_data.sa3();
    std::array<float, 35> features{new_Ff7_10, new_Ff7_20,   new_Ff7_40,   new_Ff7_80, new_Ff8_2, new_L2a1,  new_L2a2,
                                   new_L2f10,  new_L2f10_60, new_L2f10_80, new_L2f6,   new_L2f8,  new_L2f9,  new_L2p1,
                                   new_L2p2,   new_VOI,      new_a1f10,    new_a1f11,  new_a1f12, new_a1f13, new_a1f14,
                                   new_a1f15,  new_a1f4,     new_a1f6,     new_a1f7,   new_a1f9,  new_a2f12, new_a2f13,
                                   new_a2f5,   new_a2f6,     new_a2f7,     0,          0,         new_sa2,   new_sa3};
    double m_signal = ApplyCatboostModel(features);
    auto tag_end = NOW();
    LOG_INFO5("[signal] {}", m_signal);

    double new_AsyBidAsk = IC1_factor_data.AsyBidAsk;
    double new_MPB = IC1_factor_data.MPB;
    // LOG_INFO5("a1f10 = {:.5f}", new_a1f10);
    // double rank1 = rank1_caler.eval(&IC1_factor_data);
    // double rank2 = rank2_caler.eval(&IC1_factor_data);
    // if (hft::Compare(rank1, rank2) != 0) {
    //    LOG_INFO5("[rank_ERROR] rank1:{:.2f} rank2:{:.2f}", rank1, rank2);
    //}
    // double old_Ff7_10 = trigger_data->Ff7_10;
    double old_Ff8_2 = trigger_data->Ff8_2;
    double old_L2a1 = trigger_data->L2a1;
    double old_L2a2 = trigger_data->L2a2;
    double old_L2f10_60 = trigger_data->L2f10_60;
    double old_L2f6 = trigger_data->L2f6;
    double old_L2f8 = trigger_data->L2f8;
    double old_L2f1 = trigger_data->L2f1;
    double old_L2f9 = trigger_data->L2f9;
    double old_L2p1 = trigger_data->L2p1;
    double old_L2p2 = trigger_data->L2p2;
    double old_VOI = trigger_data->VOI();
    double old_AsyBidAsk = trigger_data->AsyBidAsk();
    double old_a1f10 = trigger_data->a1f10;
    double old_a1f11 = trigger_data->a1f11;
    double old_a1f12 = trigger_data->a1f12;
    double old_a1f13 = trigger_data->a1f13;
    double old_a1f14 = trigger_data->a1f14;
    double old_a1f15 = trigger_data->a1f15;
    double old_a1f4 = trigger_data->a1f4;
    double old_a1f5 = trigger_data->a1f5;
    double old_a1f6 = trigger_data->a1f6;
    double old_a1f7 = trigger_data->a1f7;
    double old_a1f9 = trigger_data->a1f9;
    double old_MPB = trigger_data->MPB();

    double old_a2f5 = trigger_data->a2f5;
    double old_a2f6 = trigger_data->a2f6;
    double old_a2f7 = trigger_data->a2f7;
    double old_a2f12 = trigger_data->a2f12;
    double old_a2f13 = trigger_data->a2f13;
    auto old_sa2 = trigger_data->sa2;
    auto old_sa3 = trigger_data->sa3;
    if (hft::Compare(new_sa2, old_sa2) != 0) {
        LOG_INFO5("[sa2_ERROR] old:{:.2f} new:{:.2f}", old_sa2, new_sa2);
    }
    if (hft::Compare(new_sa3, old_sa3) != 0) {
        LOG_INFO5("[sa2_ERROR] old:{:.2f} new:{:.2f}", old_sa3, new_sa3);
    }
    if (hft::Compare(old_MPB, new_MPB) != 0) {
        LOG_INFO5("[MPB_ERROR] old:{:.2f} new:{:.2f}", old_MPB, new_MPB);
    }
    if (hft::Compare(old_L2f1, new_L2f1) != 0) {
        LOG_INFO5("[L2f1_ERROR] old:{:.2f} new:{:.2f}", old_L2f1, new_L2f1);
    }
    if (hft::Compare(old_a1f10, new_a1f10) != 0) {
        LOG_INFO5("[a1f10_ERROR] old:{:.5f} new:{:.5f}", old_a1f10, new_a1f10);
    }
    if (hft::Compare(old_a1f11, new_a1f11) != 0) {
        LOG_INFO5("[a1f11_ERROR] old:{:.2f} new:{:.2f}", old_a1f11, new_a1f11);
    }
    if (hft::Compare(old_a1f12, new_a1f12) != 0) {
        LOG_INFO5("[a1f12_ERROR] old:{:.2f} new:{:.2f}", old_a1f12, new_a1f12);
    }
    if (hft::Compare(old_a1f13, new_a1f13) != 0) {
        LOG_INFO5("[a1f13_ERROR] old:{:.2f} new:{:.2f}", old_a1f13, new_a1f13);
    }
    if (hft::Compare(old_a1f14, new_a1f14) != 0) {
        LOG_INFO5("[a1f14_ERROR] old:{:.2f} new:{:.2f}", old_a1f14, new_a1f14);
    }
    if (hft::Compare(old_a1f15, new_a1f15) != 0) {
        LOG_INFO5("[a1f15_ERROR] old:{:.2f} new:{:.2f}", old_a1f15, new_a1f15);
    }

    if (hft::Compare(old_a1f5, new_a1f5) != 0) {
        LOG_INFO5("[a1f5_ERROR] old:{:.2f} new:{:.2f}", old_a1f5, new_a1f5);
    }

    if (hft::Compare(old_a1f4, new_a1f4) != 0) {
        LOG_INFO5("[a1f4_ERROR] old:{:.2f} new:{:.2f}", old_a1f4, new_a1f4);
    }

    if (hft::Compare(old_a1f6, new_a1f6) != 0) {
        LOG_INFO5("[a1f6_ERROR] old:{:.2f} new:{:.2f}", old_a1f6, new_a1f6);
    }

    if (hft::Compare(old_a1f7, new_a1f7) != 0) {
        LOG_INFO5("[a1f7_ERROR] old:{:.2f} new:{:.2f}", old_a1f7, new_a1f7);
    }

    if (hft::Compare(old_a1f9, new_a1f9) != 0) {
        LOG_INFO5("[a1f9_ERROR] old:{:.2f} new:{:.2f}", old_a1f9, new_a1f9);
    }
    if (hft::Compare(old_a2f5, new_a2f5) != 0) {
        LOG_INFO5("[a2f5_ERROR] old:{:.2f} new:{:.2f}", old_a2f5, new_a2f5);
    }

    if (hft::Compare(old_a2f6, new_a2f6) != 0) {
        LOG_INFO5("[a2f6_ERROR] old:{:.2f} new:{:.2f}", old_a2f6, new_a2f6);
    }

    if (hft::Compare(old_a2f7, new_a2f7) != 0) {
        LOG_INFO5("[a2f7_ERROR] old:{:.2f} new:{:.2f}", old_a2f7, new_a2f7);
    }

    if (hft::Compare(old_a2f12, new_a2f12) != 0) {
        LOG_INFO5("[a2f12_ERROR] old:{:.2f} new:{:.2f}", old_a2f12, new_a2f12);
    }
    if (hft::Compare(old_a2f13, new_a2f13) != 0) {
        LOG_INFO5("[a2f13_ERROR] old:{:.2f} new:{:.2f}", old_a2f13, new_a2f13);
    }

    if (hft::Compare(old_AsyBidAsk, new_AsyBidAsk) != 0) {
        LOG_INFO5("[AsyBidAsk_ERROR] old:{:.2f} new:{:.2f}", old_AsyBidAsk, new_AsyBidAsk);
    }
    if (hft::Compare(old_VOI, new_VOI) != 0) {
        LOG_INFO5("[VOI_ERROR] old:{:.2f} new:{:.2f}", old_VOI, new_VOI);
    }
    if (hft::Compare(old_L2p2, new_L2p2) != 0) {
        LOG_INFO5("[L2p2_ERROR] old:{:.2f} new:{:.2f}", old_L2p2, new_L2p2);
    }
    if (hft::Compare(old_L2p1, new_L2p1) != 0) {
        LOG_INFO5("[L2p1_ERROR] old:{:.2f} new:{:.2f}", old_L2p1, new_L2p1);
    }
    if (hft::Compare(old_L2f9, new_L2f9) != 0) {
        LOG_INFO5("[L2f9_ERROR] old:{:.2f} new:{:.2f}", old_L2f9, new_L2f9);
    }
    if (hft::Compare(old_L2f6, new_L2f6) != 0) {
        LOG_INFO5("[L2f6_ERROR] old:{:.2f} new:{:.2f}", old_L2f6, new_L2f6);
    }
    if (hft::Compare(old_L2f8, new_L2f8) != 0) {
        LOG_INFO5("[L2f8_ERROR] old:{:.2f} new:{:.2f}", old_L2f8, new_L2f8);
    }
    if (hft::Compare(old_L2f10_60, new_L2f10_60) != 0) {
        LOG_INFO5("[L2f10_60] old:{:.5f} new:{:.5f}", old_L2f10_60, new_L2f10_60);
    }

    if (hft::Compare(old_L2a2, new_L2a2) != 0) {
        LOG_INFO5("[L2a2_ERROR] old:{:.5f} new:{:.5f}", old_L2a2, new_L2a2);
    }

    if (hft::Compare(old_Ff8_2, new_Ff8_2) != 0) {
        LOG_INFO5("[Ff8_2_ERROR] old:{:.5f} new:{:.5f}", old_Ff8_2, new_Ff8_2);
    }

    auto dur = DURATION_ns(tag_start, tag_end);
    dur_list.push_back(dur);
    if (dur_list.size() > 1000000) {
        double mean{0}, std{0};
        mean = math_tool::get_mean(dur_list.begin(), dur_list.end());
        std = math_tool::get_std(dur_list.begin(), dur_list.end());
        printf("[BENCK_MARK] mean %.2f +/- %.2f\n", mean - 16, std);
        exit(0);
    }

    double old_PIM =
        (trigger_data->tp_list.pre(0) - 0.5 * (trigger_data->mp.pre(0) + trigger_data->mp.pre(1))) / trigger_data->sp;
    double now_PIM = IC1_factor_data.PIMsp;
    if (hft::Compare(trigger_data->tp_list.pre(0), IC1_factor_data.wvap) != 0) {
        LOG_INFO3("[wvap_ERROR] old:{:.5f} new:{:.5f}", trigger_data->tp_list.pre(0), IC1_factor_data.wvap);
    }
    // LOG_INFO3("[PIMsp] old:{:.5f} new:{:.5f}", old_PIM, now_PIM);
    if (hft::Compare(old_PIM, now_PIM) != 0) {
        LOG_INFO5("[PIMsp_ERROR] old:{:.5f} new:{:.5f}", old_PIM, now_PIM);
    }
    double mt_old = trigger_data->tp_list.pre(0) - 0.5 * (trigger_data->mp.pre(0) + trigger_data->mp.pre(1));
    double mt_new = IC1_factor_data.PIM;
    if (hft::Compare(mt_old, mt_new) != 0) {
        LOG_INFO5("[PIM_ERROR] old:{:.5f} new:{:.5f}", mt_old, mt_new);
    }
    if (hft::Compare(old_L2a1, new_L2a1) != 0) {
        LOG_INFO5("[L2a1_ERROR] old:{:.5f} new:{:.5f}", old_L2a1, new_L2a1);
    }
    auto old_bid_amount = trigger_data->bid_amount;
    auto new_bid_amount = IC1_factor_data.bid_amount;
    auto old_ask_amount = trigger_data->ask_amount;
    auto new_ask_amount = IC1_factor_data.ask_amount;
    LOG_INFO3("[bid_amount] old:{:.4f} new:{:.4f}", old_bid_amount, new_bid_amount);
    if (hft::Compare(old_bid_amount, new_bid_amount) != 0) {
        LOG_INFO5("[bid_amount_ERROR] old:{:.4f} new:{:.4f}", old_bid_amount, new_bid_amount);
    }
    if (hft::Compare(old_ask_amount, new_ask_amount) != 0) {
        LOG_INFO5("[ask_amount_ERROR] old:{:.4f} new:{:.4f}", old_ask_amount, new_ask_amount);
    }
    LOG_INFO1("signal {}", m_signal);

    // after pose
    this->doExPose();
    LOG_INFO1("[{}] DONE", __FUNCTION__);
    // t_IC1 = GetTimeStemp(tick)      ;
    //    LOG_INFO("time:{}", t_IC1)  ;
    // t_IC2 = GetTimeStemp(tick_IC2);
    // t_IF1 = GetTimeStemp(tick_IF1);
    // t_IH1 = GetTimeStemp(tick_IF1);
}
void Strapro::doExPose() {
    IC1_factor_data.ExPose(IC1(0));
    // Ff7_caler.ExPose(&IC1_factor_data);
    Ff7_10_caler.ExPose(&IC1_factor_data);
    Ff7_20_caler.ExPose(&IC1_factor_data);
    Ff7_40_caler.ExPose(&IC1_factor_data);
    Ff7_80_caler.ExPose(&IC1_factor_data);
    Ff8_2_caler.ExPose(&IC1_factor_data);
    L2a1_caler.ExPose(&IC1_factor_data);
    L2a2_caler.ExPose(&IC1_factor_data);
    L2f10_10_caler.ExPose(&IC1_factor_data);
    L2f10_10_caler.ExPose(&IC1_factor_data);
    L2f10_20_caler.ExPose(&IC1_factor_data);
    L2f10_40_caler.ExPose(&IC1_factor_data);
    L2f10_60_caler.ExPose(&IC1_factor_data);
    L2f10_80_caler.ExPose(&IC1_factor_data);
    L2f9_caler.ExPose(&IC1_factor_data);
    L2p1_caler.ExPose(&IC1_factor_data);
    L2p2_caler.ExPose(&IC1_factor_data);
    VOI_caler.ExPose(&IC1_factor_data);
    a1f10_caler.ExPose(&IC1_factor_data);
    a1f11_caler.ExPose(&IC1_factor_data);
    rank1_caler.ExPose(&IC1_factor_data);
    rank2_caler.ExPose(&IC1_factor_data);
}
