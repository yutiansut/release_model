#pragma once
#include "hft/List.hpp"
#include "hft/base.h"
namespace hft {
// 一个class用来计算指数移动平均, 只计算增量，主要的目的是提供计算效率
// 计算方式
// mean(t) =  alpha * x(t) + (1 - alpha) * mean(t-1)
template <typename T>
class EWMCaler {
   public:
    explicit EWMCaler(const T& alpha);

    inline T GetMean() const { return m_mean; }
    // 增加一个值用来做计算
    void Add(const T& a_value);
    void SetAlpha(const T& alpha) { m_alpha = alpha; }
    void ReSet();
    bool Valid() const { return !first_init; }

   private:
    T m_alpha;
    T m_mean;

   private:
    bool first_init;
};

template <typename T>
EWMCaler<T>::EWMCaler(const T& alpha) : m_alpha{alpha}, m_mean{0}, first_init{true} {}

template <typename T>
void EWMCaler<T>::ReSet() {
    first_init = true;
    // m_mean = T{0};
}
template <typename T>
void EWMCaler<T>::Add(const T& a_value) {
    if (UNLIKELY(first_init)) {
        m_mean = a_value;
        first_init = false;
    } else {
        m_mean = m_alpha * a_value + (1 - m_alpha) * m_mean;
    }
}
};  // namespace hft
