#include "mydefine.h"

//#define LOG_LEVEL_2
//
#include "BDT_model.hpp"
#include "StraPro.hpp"
#include "alpha/alpha.h"
#include "hft/FactorMan.h"
#include "hft/FuturesPairInfo.hpp"
#include "hft/MarketData.h"
#include "hft/base.h"
using namespace std;    // NOLINT
using namespace husky;  // NOLINT

static size_t counter{0};
static int32_t trade_mode;
static int32_t max_pos;
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
}
void Strapro::re_set_data() {
    if (use_IC1) IC1_data.ReSet();
    if (use_IC2) IC2_data.ReSet();
    if (use_IF1) IF1_data.ReSet();
    if (use_IF2) IF2_data.ReSet();
    if (use_IH1) IH1_data.ReSet();
    if (use_IH2) IH2_data.ReSet();
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
    for (uint32_t i = 0; i < init_ticks; ++i) {
        int cur_tick = first_tick + i;
        if (use_IC1) IC1_data.AppendTick(IC1(cur_tick));
        if (use_IC2) IC2_data.AppendTick(IC2(cur_tick));
        if (use_IF1) IF1_data.AppendTick(IF1(cur_tick));
        if (use_IF2) IF2_data.AppendTick(IF2(cur_tick));
        if (use_IH1) IH1_data.AppendTick(IH1(cur_tick));
        if (use_IH2) IH2_data.AppendTick(IH2(cur_tick));
        ++counter;
    }
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
    LOG_INFO2("===================={}==============", ++counter);
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

#include "use_factor.h"
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

#include "signal.h"

#include "threshold.txt"

    LOG_INFO1("signal {}", m_signal);
    if (m_signal >= m_threshold1) {
        LOG_INFO2("buy signal {}", m_signal);
        target_pos = this->get_net_position() + 1;
        if (target_pos == 0) {
            target_pos++;
        }
        target_pos = MIN(target_pos, max_pos);
        m_orders = 3;
        LOG_INFO1("NetPositions {} target_pos {}", get_net_position(), target_pos);
        auto bid1 = trigger_tick->operator()(0).bid[0];
        auto ask1 = trigger_tick->operator()(0).ask[0];
        LOG_INFO1("bid:{} ask:{}", bid1, ask1);
        // HUSKY_TRADE_INFO("target_pos:{} net_pos:{}", target_pos,
        // this->get_net_position());
        if (target_pos - this->get_net_position() > 0) {
            NewBuyOrder(target_pos - this->get_net_position(), trade_mode);
        }
        // this->TradeOrder();
    } else if (m_signal <= m_threshold2) {
        LOG_INFO2("sell signal {}", m_signal);
        // target_pos = -1;
        target_pos = this->get_net_position() - 1;
        if (target_pos == 0) target_pos--;
        target_pos = MAX(target_pos, -max_pos);
        m_orders = 3;
        auto bid1 = trigger_tick->operator()(0).bid[0];
        auto ask1 = trigger_tick->operator()(0).ask[0];
        LOG_INFO1("#{} bid:{} ask:{}", counter, bid1, ask1);
        LOG_INFO1("#{} NetPositions {} target_pos {}", counter, get_net_position(), target_pos);
        // HUSKY_TRADE_INFO("target_pos:{} net_pos:{}", target_pos,
        // this->get_net_position());
        if (target_pos - this->get_net_position() < 0) {
            NewSellOrder(this->get_net_position() - target_pos, trade_mode);
        }
        // this->TradeOrder();
    }
    LOG_INFO1("[{}] DONE", __FUNCTION__);
    // t_IC1 = GetTimeStemp(tick)      ;
    //    LOG_INFO("time:{}", t_IC1)  ;
    // t_IC2 = GetTimeStemp(tick_IC2);
    // t_IF1 = GetTimeStemp(tick_IF1);
    // t_IH1 = GetTimeStemp(tick_IF1);
}
