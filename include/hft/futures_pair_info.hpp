#pragma once
#include "hft/FactorMan.h"
#include "hft/factor_func.hpp"
namespace hft {
namespace factor {
double FPA1(FactorMan *df1, FactorMan *df2, const uint16_t &period) {
  auto dB = df2->bid_vol_list - df1->bid_vol_list;
  auto dA = df2->ask_vol_list - df1->ask_vol_list;
  auto &dp = df2->mp - df1->mp;
  return ff::ts_trend(dB, period) - ff::ts_trend(dA, period) +
         ff::ts_trend(dp, period);
}

double FPA2(FactorMan *df1, FactorMan *df2, const uint16_t &span) {
  auto ih_ret = ff::ts_delta(ff::ts_log(df2->mp), 1);
  auto ic_ret = ff::ts_delta(ff::ts_log(df1->mp), 1);
  auto &dp = ih_ret - ic_ret;
  return ff::ts_mean(dp, span) * 10000;
}

double FPA3(FactorMan *df1, FactorMan *df2, const uint16_t &period) {
  auto deltaP = df2->mp - df1->mp;
  return ff::ts_trend2(deltaP, period);
}

double FPC1(FactorMan *df_main, FactorMan *df_assi,
            const uint32_t &period = 20);
double FPC2(FactorMan *df_main, FactorMan *df_assi,
            const uint32_t &period = 20);
double FPC3(FactorMan *df_main, FactorMan *df_assi,
            const uint32_t &period = 20);
double FPC4(FactorMan *df_main, FactorMan *df_assi,
            const uint32_t &period = 20);
// 价差的trend
double FPD1(FactorMan *df_main, FactorMan *df_assi,
            const uint32_t &period = 20);
// 价差的trend2
double FPD2(FactorMan *df_main, FactorMan *df_assi,
            const uint32_t &period = 20);
// 价差加速
double FPD3(FactorMan *df_main, FactorMan *df_assi);
// 价比的trend
double FPD4(FactorMan *df_main, FactorMan *df_assi,
            const uint32_t &period = 20);
}; // namespace factor
}; // namespace hft
