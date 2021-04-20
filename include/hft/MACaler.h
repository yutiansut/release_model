#pragma once
namespace hft {
// 计算移动平均值
template <typename T>
class MACaler {
   public:
    MACaler();

   private:
    T m_mean;
    T m_std;
};

};  // namespace hft
