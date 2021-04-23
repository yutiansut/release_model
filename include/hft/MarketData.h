#pragma once
// MarketData 用来存储行情，附带一些实现函数
//
#include "common/define.h"
#include "common/log.h"
#include "hft/List.hpp"
#include "hft/base.h"
#include <iostream>
namespace hft {
class MarketData {
   public:
    explicit MarketData(const uint32_t& min_length);
    // void SetTickerInfo(const md::ticker_info& tickerinfo) { m_tick_info = tickerinfo; }
    void SetMultiplier(const double multiplier) { m_tick_info.multiplier = multiplier; }
    void AppendTick(const md::l2& tick);
    virtual void Calculate() {}
    // 事前处理，要求速度快，内存读取次数少
    virtual void PreProcessTick(const md::l2& tick) {}
    void ReSet();  // 重新初始化
    size_t TickeLength() const { return last_price.cum_size(); }

   public:
    List<double> last_price;
    List<double> lastamount;  // turnover
    List<double> lastopenint;
    List<double> turn_over;
    List<int32_t> volume;
    List<double> mp;
    List<double> tp_list;
    List<double> bid[5];
    List<double> ask[5];
    List<double> sp_list;
    List<int32_t> bidv[5];
    List<int32_t> askv[5];
    List<double> delta_bidv1, delta_askv1;
    double m_lastvolume;
    double* last_price_itr;
    double* lastamount_itr;
    double* lastopenint_itr;
    double* turn_over_itr;
    int32_t* volume_itr;
    double* mp_itr;
    double* TP_itr;
    double* bid_itr[5];
    double* ask_itr[5];
    int32_t* askv_itr[5];
    int32_t* bidv_itr[5];

   public:
    // 一些常用的变量
    double ask1, bid1, tp, sp, mid_price;
    int32_t askv1, bidv1, trade_vol;

   private:
    md::ticker_info m_tick_info;
};

MarketData::MarketData(const uint32_t& min_length)
    : last_price{min_length},
      lastamount{min_length},
      lastopenint{min_length},
      turn_over{min_length},
      volume{min_length},
      mp{min_length},
      tp_list{min_length},
      bid{List<double>{min_length}, List<double>{min_length}, List<double>{min_length}, List<double>{min_length},
          List<double>{min_length}},
      ask{List<double>{min_length}, List<double>{min_length}, List<double>{min_length}, List<double>{min_length},
          List<double>{min_length}},
      sp_list{min_length},
      bidv{List<int32_t>{min_length}, List<int32_t>{min_length}, List<int32_t>{min_length}, List<int32_t>{min_length},
           List<int32_t>{min_length}},
      askv{List<int32_t>{min_length}, List<int32_t>{min_length}, List<int32_t>{min_length}, List<int32_t>{min_length},
           List<int32_t>{min_length}},
      delta_bidv1{min_length},
      delta_askv1{min_length},
      m_lastvolume{0},
      last_price_itr{nullptr},
      lastamount_itr{nullptr},
      lastopenint_itr{nullptr},
      turn_over_itr{nullptr},
      volume_itr{nullptr},
      mp_itr{nullptr},
      TP_itr{nullptr},
      bid_itr{nullptr, nullptr, nullptr, nullptr, nullptr},
      ask_itr{nullptr, nullptr, nullptr, nullptr, nullptr},
      askv_itr{nullptr, nullptr, nullptr, nullptr, nullptr},
      bidv_itr{nullptr, nullptr, nullptr, nullptr, nullptr},
      ask1{0},
      bid1{0},
      tp{0},
      sp{0},
      mid_price{0},
      askv1{0},
      bidv1{0},
      trade_vol{0},
      m_tick_info{"IC", "IC1", 200, 0.2} {

    volume_itr = const_cast<int32_t*>(volume.Append(0));
}

void MarketData::AppendTick(const md::l2& tick) {
    last_price_itr = const_cast<double*>(last_price.Append(tick.lastprice));
    lastamount_itr = const_cast<double*>(lastamount.Append(tick.lastamount));
    lastopenint_itr = const_cast<double*>(lastopenint.Append(tick.lastopenint));
    turn_over_itr = const_cast<double*>(turn_over.Append(tick.lastamount - *(lastamount_itr - 1)));
    trade_vol = tick.lastvolume - m_lastvolume;
    volume_itr = const_cast<int32_t*>(volume.Append(trade_vol));
    LOG_INFO1("[MarketData::AppendTick] lastvolume {} trade_vol {}", m_lastvolume, *volume_itr);
    m_lastvolume = tick.lastvolume;

    for (int i = 0; i < 5; ++i) {
        bid_itr[i] = const_cast<double*>(bid[i].Append(tick.bid[i]));
        ask_itr[i] = const_cast<double*>(ask[i].Append(tick.ask[i]));
        bidv_itr[i] = const_cast<int32_t*>(bidv[i].Append(tick.bidv[i]));
        askv_itr[i] = const_cast<int32_t*>(askv[i].Append(tick.askv[i]));
    }
    sp_list.Append(tick.ask[0] - tick.bid[0]);
    // cal mp
    mp_itr = const_cast<double*>(mp.Append(0.5 * (*(bid_itr[0]) + *(ask_itr[0]))));

    // calculate the trade average price
    if (UNLIKELY(trade_vol == 0)) {
        tp = tick.lastprice;
    } else {
        tp = (*turn_over_itr) / trade_vol / m_tick_info.multiplier;
    }
    LOG_INFO("[multiplier {}]", m_tick_info.multiplier);
    TP_itr = const_cast<double*>(tp_list.Append(tp));
    ask1 = tick.ask[0];
    bid1 = tick.bid[0];
    sp = ask1 - bid1;
    mid_price = *mp_itr;
    askv1 = tick.askv[0];
    bidv1 = tick.bidv[0];
    // printf("%s %d ask1: %.2f\n", __FUNCTION__, __LINE__, ask1);
    Calculate();
}

void MarketData::ReSet() {
    last_price.ReSet();
    lastamount.ReSet();
    lastopenint.ReSet();
    turn_over.ReSet();
    volume.ReSet();
    mp.ReSet();
    tp_list.ReSet();
    for (int i = 0; i < 5; ++i) {
        bid[i].ReSet();
        ask[i].ReSet();
        askv[i].ReSet();
        bidv[i].ReSet();
    }
    m_lastvolume = 0;
    LOG_INFO("MarketData::ReSet m_lastvolume{}", m_lastvolume);
}

};  // namespace hft
