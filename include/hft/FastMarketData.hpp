#pragma once
// FastMarketData 用来存储行情，附带一些实现函数
//
#include "common/define.h"
#include "common/log.h"
#include "hft/List.hpp"
#include "hft/base.h"
#include <iostream>
namespace hft {
class FastMarketData {
   public:
    explicit FastMarketData(const uint32_t& min_length);
    // void SetTickerInfo(const md::ticker_info& tickerinfo) { m_tick_info = tickerinfo; }
    void SetMultiplier(const double multiplier) { m_tick_info.multiplier = multiplier; }
    void AppendTick(const md::l2& tick);
    void initialize();
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

};  // namespace hft
