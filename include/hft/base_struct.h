#pragma once
#include "hft/List.h"
#include "hft/base.h"
#include <string>
namespace hft {
template <class T>
struct V2 {
    T x;
    T y;
};
template <class T>
struct V3 {
    T x;
    T y;
    T z;
};

// Tintput 原始数据类型， Toutput 输出类型 Inc 增量类型 period 计算周期
template <class Tintput, class Toutput, class Inc, int period = 20>
class IncrementalCompute {
    // 增量式计算
   public:
    IncrementalCompute() {}
    void Append(const Tintput& x) {
        m_values.Append(x);
        cal_current_value(x, &cur_val);
    }

    void ExPost() {
        cal_previous_value(m_values, &pre_val);
        m_values.ExPost();
    }

   private:
    virtual void cal_previous_value(const List<Tintput>&, Inc*) = 0;
    virtual void cal_current_value(const Tintput&, Inc*) = 0;
    virtual Toutput value(const Inc& pre_val, const Inc& cur_val) = 0;

   private:
    // 历史值
    List<Tintput, 5 * period> m_values{period};
    // 前值
    Inc pre_val;
    // 现值
    Inc cur_val;
};

};  // namespace hft
