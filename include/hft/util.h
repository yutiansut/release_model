#ifndef KERNEL_STRATEGY_STRATS_MAXX_TRADE_TOOL_UTIL_H_
#define KERNEL_STRATEGY_STRATS_MAXX_TRADE_TOOL_UTIL_H_
#include "base/util.h"
#include "kernel/redis/redis_man.h"
#include "kernel/strategy/strat_orderdata.h"
#include "struct/instrument_detail.h"
#include "struct/market/market_data.h"
#include <cmath>
#define Integer_1M 1000000
#define Integer_1K 1000
#define Integer_1k 0.001
#define Integer_1m 0.000001
#define T_1ms 1000
#define T_One_second 1000000
#define T_1s 1000000
namespace trade_tool {
double round3(const double& price) { return round(price * 1e3) * 1e-3; }
double round2(const double& price) { return round(price * 1e2) * 1e-2; }

template <typename bar>
void to_standard_bar(const hft::MarketData& md, bar& output) {
    output.kline.open = md.open_price_;
    output.kline.close = md.close_price_;
    output.kline.high = md.high_price_;
    output.kline.low = md.low_price_;
    output.kline.volume = md.volume_;
}
void swap(hft::StratOrderData** a, hft::StratOrderData** b) {
    hft::StratOrderData* tmp = *a;
    *a = *b;
    *b = tmp;
}
void Format(double& price, const double& m_tick_size) { price = m_tick_size * static_cast<int>(price / m_tick_size); }
#define FormatPrice(price, tick) price = tick * static_cast<int>(price / tick);
// static hft::RedisMan rm;
bool GetKLine(const hft::InstrumentDetail* dtl, hft::QuoteOkexTopic period, int num,
              std::vector<hft::MarketData>& datas) {
    static hft::RedisMan rm;
    //
    int ret = rm.Connect("localhost");
    // int ret = rm.ConnectWithPasswd("8.210.65.60", 6379, "666666");
    if (ret == hft::kRecSuccess) {
        // ret = rm.GetCoinHisKLine("okex.future.kline.1m.eos-usd-nextqua", kQotCand60, -2000, datas);
        ret = rm.GetCoinHisKLine(dtl, period, num, datas);
        // simulator暂时不支持下面的用法
        // ret = rm.GetCoinHisKLine(instrument(INST_TRADE), kQotCand60,
        // -1000, datas);

        return ret == hft::kRecSuccess;
    } else {
        LOG_INFO("Can't get the kline'");
        return false;
    }
}
int xsqsum(int n) { return n * (n + 1) * (2 * n + 1) / 6; }
int xsum(int n) { return (n + 1) * n / 2; }
template <typename TItr, typename T>
bool slope(TItr begin, TItr end, T& mean) {
    // 对包含begin和end在内的数组进行拟合
    T y_sum = 0.0;
    T xy_sum = 0.0;
    for (auto itr = begin; itr <= end; itr++) {
        xy_sum += (itr - begin) * (*itr);
        // #ifdef DEBUG_STRATS
        //         LOG_INFO("i = [%ld], y = %.3f", itr - begin, *itr);
        // #endif
        y_sum += *itr;
    }
    int n = end - begin;
    int x_sum = xsum(n);
    int xsq_sum = xsqsum(n);
    n += 1;
    mean = (n * xy_sum - x_sum * y_sum) / (n * xsq_sum - x_sum * x_sum);
    return true;
}
void BuyCoin(hft::StrategyInterface* si, const double& price, const size_t& size, const hft::Market& m,
             const size_t& index = 0) {
    si->NewBidOrder(index, price, size, m, 0, hft::kOttMarket, false, true, false);
}
}  // namespace trade_tool
#endif  // KERNEL_STRATEGY_STRATS_MAXX_TRADE_TOOL_UTIL_H_
