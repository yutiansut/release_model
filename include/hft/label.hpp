#pragma once
#include "hft/MarketData.h"
#include "hft/TProduct.hpp"
#include "hft/base.h"
#include "hft/factor_func.hpp"
namespace hft {
class Label : public MarketData, public TProduct {
   public:
    // name 是合约的简称名，如IF1, IH2
    // min_length 是计算label所需的最少的ticks个数
    Label(const std::string& name, const uint32_t& min_length)
        : MarketData{min_length},
          TProduct{name},
          RawReturn20{0},
          RawReturn40{0},
          RawReturn60{0},
          RawReturn80{0},
          log_mp_list{min_length},
          mp_ret_list{min_length},
          m_name{name},
          m_future_ticks{0},
          m_min_length{min_length} {}
    void set_use_future_ticks(uint32_t future_ticks) { m_future_ticks = future_ticks; }

    void ReSet() {
        log_mp_list.ReSet();
        mp_ret_list.ReSet();
    }
    void Calculate() override {
        pre_data();
        cal_labels();
    }

   public:
    inline void pre_data() {
        // 计算价格的对数
        double current_mp = mp.pre(0);
        log_mp_list.Append(log(current_mp));
        mp_ret_list.Append(log_mp_list.pre(0) - log_mp_list.pre(1));
    }
    // 需要至少period+1个tick
    inline double RawReturn(uint32_t period = 20) {
        double current_mp = mp.pre(m_future_ticks);
        double MA_mp{0};
        double* cur_mp = mp.itr() - m_future_ticks;
        math_tool::Mean(cur_mp + period, period, &MA_mp);
        return MA_mp - current_mp;
    }
    inline void cal_labels() {
#ifdef CAL_RawReturn20
        RawReturn20 = RawReturn(20);
#endif
#ifdef CAL_RawReturn40
        RawReturn40 = RawReturn(40);
#endif
#ifdef CAL_RawReturn60
        RawReturn60 = RawReturn(60);
#endif
#ifdef CAL_RawReturn80
        RawReturn80 = RawReturn(80);
#endif
    }

   public:
    // 一些现成的因子
    double RawReturn20;
    double RawReturn40;
    double RawReturn60;
    double RawReturn80;
    // 未来收益的夏普
    inline double SR(uint32_t period) {
        double mean_returns{0};
        double std_returns{0};
        double* cur_ret = mp_ret_list.itr() - m_future_ticks;
        math_tool::Mean(cur_ret + period, period, &mean_returns);
        math_tool::Var(cur_ret + period, period, &std_returns);
        if (fabs(std_returns) < 1e-5) {
            return 0;
        }
        return mean_returns / std_returns;
    }
    //
    // def SR(tk: pd.DataFrame, timeperiod: int = 20):
    //    """ 收益/标准差 """
    //    df = tk.copy()
    //    df['logP'] = np.log(df.TP.values)
    //    df['Return'] = df.logP.shift(-1) - df.logP
    //    roll = df.Return.rolling(timeperiod)
    //    df['SR'] = (roll.mean() / roll.std()).replace([np.inf, -np.inf], 0)
    //    return df.SR.shift(-timeperiod)
   public:
    List<double, 200> log_mp_list;
    List<double, 200> mp_ret_list;

   private:
    std::string m_name;
    uint32_t m_future_ticks;
    uint32_t m_min_length;
};
};  // namespace hft
