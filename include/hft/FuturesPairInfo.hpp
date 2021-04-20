#pragma once
#include "hft/FactorMan.h"
#include "hft/factor_func.hpp"
namespace hft {
// try to get the combination factor of two constracts
void fpBase1(const FactorMan &df1, const FactorMan &df2, const uint16_t &period, double *output) {
    auto dB = df2.bid_vol_list - df1.bid_vol_list;
    auto dA = df2.ask_vol_list - df1.ask_vol_list;
    auto &dp = df2.mp - df1.mp;
    *output = ff::ts_trend(dB, period) - ff::ts_trend(dA, period) + ff::ts_trend(dp, period);
}

void fpBase2(const FactorMan &df1, const FactorMan &df2, const uint16_t &span, double *output) {
    auto ih_ret = ff::ts_delta(ff::ts_log(df2.mp), 1);
    auto ic_ret = ff::ts_delta(ff::ts_log(df1.mp), 1);
    auto &dp = ih_ret - ic_ret;
    // double alpha = 2.0 / (1 + span);
    *output = ff::ts_mean(dp, span) * 10000;
}

void fpBase3(const FactorMan &df1, const FactorMan &df2, const uint16_t &period, double *output) {
    auto deltaP = df2.mp - df1.mp;
    *output = ff::ts_trend2(deltaP, period);
    //*output = ff::ts_trend2(df2.mp, period) - ff::ts_trend2(df1.mp, period);
    // *output = df2.mp.pre(0);
}

struct fpfactor {
    double fpf1;
    double fpf2;
    double fpf3;
    double fpf4;
    double fpf5;
    double fpf6;
    double fpf7;
    double fpf8;
    double fpf9;
    double fpf10;
    double fpf11;
    double fpf12;
    double fpf13;
    double fpf14;
    double fpf15;
    double fpf16;
    double fpf17;
    double fpf18;
};
};  // namespace hft
