#include "hft/futures_pair_info.hpp"
//#include "hft/FactorMan.h"
#include "hft/factor_func.hpp"
namespace hft {
namespace factor {
double FPC1(FactorMan *df_main, FactorMan *df_assi, const uint32_t &period) {
    // 买入和卖出的趋势之差
    auto trend1 = ff::ts_trend(df_main->ask_vol_list, period);
    auto trend2 = ff::ts_trend(df_assi->bid_vol_list, period);
    return trend1 - trend2;
}

double FPC2(FactorMan *df_main, FactorMan *df_assi, const uint32_t &period) {
    auto trend1 = ff::ts_trend(df_main->bid_vol_list, period);
    auto trend2 = ff::ts_trend(df_assi->ask_vol_list, period);
    return trend1 - trend2;
}

double FPC3(FactorMan *df_main, FactorMan *df_assi, const uint32_t &period) {
    auto trend1 = ff::ts_trend2(df_main->bid_vol_list, period);
    auto trend2 = ff::ts_trend2(df_assi->ask_vol_list, period);
    return trend1 - trend2;
}

double FPC4(FactorMan *df_main, FactorMan *df_assi, const uint32_t &period) {
    auto trend1 = ff::ts_trend2(df_main->ask_vol_list, period);
    auto trend2 = ff::ts_trend2(df_assi->bid_vol_list, period);
    return trend1 - trend2;
}

double FPD1(FactorMan *df_main, FactorMan *df_assi, const uint32_t &period) {
    auto dp = df_main->mp - df_assi->mp;
    dp = dp - ts_Mean(dp, period * 2);
    auto tr_ass = ff::ts_trend(df_assi->mp, period);
    auto tr = ff::ts_trend(df_main->mp, period);
    if (dp.pre(0) * tr_ass > 0) {
        return tr_ass;
    } else {
        return tr;
    }
}

double FPD2(FactorMan *df_main, FactorMan *df_assi, const uint32_t &period) {
    auto dp = df_main->mp - df_assi->mp;
    dp = dp - ts_Mean(dp, period * 2);
    auto tr_ass = ff::ts_trend2(df_assi->mp, period);
    auto tr = ff::ts_trend2(df_main->mp, period);
    if (dp.pre(0) * tr_ass > 0) {
        return tr_ass;
    } else {
        return tr;
    }
}

double FPD3(FactorMan *df_main, FactorMan *df_assi) {
    auto pr = ff::ts_log(ff::ts_div(df_main->mp, df_assi->mp)) * 1e3;
    auto MA1 = ff::ts_mean(pr, 5);
    auto MA2 = ff::ts_mean(pr, 20);
    auto MA3 = ff::ts_mean(pr, 40);
    return MA1 - 2 * MA2 + MA3;
}

double FPD4(FactorMan *df_main, FactorMan *df_assi, const uint32_t &period) {
    auto pr = ff::ts_log(ff::ts_div(df_main->mp, df_assi->mp)) * 1e3;
    return ff::ts_trend(pr, period);
}
};  // namespace factor
};  // namespace hft
