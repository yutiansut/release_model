#pragma once
// #include <iostream>
#include "hft/base.h"
#include "hft/math_tool.hpp"
namespace hft {

template <typename T, uint32_t LENGTH = 400>
class List {
   public:
    explicit List(const uint32_t& min_length = 125);

   public:
    void SetMinLength(const uint32_t& min_length) { m_min_length = min_length; }
    uint32_t GetMinLength() const { return m_min_length; }
    const T* Append(const T& value);
    // 只提供最新的信息
    const T* begin() const;
    const T* end() const;
    T& operator[](size_t i);
    const T& operator[](size_t i) const;
    // 当前数据的指针, 最后Append的元素的地址
    T* itr() { return m_itr - 1; }
    T pre(const uint32_t& i = 0) const { return *(m_itr - i - 1); }
    size_t size() const { return LENGTH; }
    size_t cum_size() const { return m_cum_size; }
    size_t cur_size() const { return static_cast<size_t>(m_itr - m_list); }
    void ReSet() { m_cum_size = 0; }
    explicit operator int();

    // 如果不重载赋值运算符，容易引起内存管理问题
    List<T, LENGTH>& operator=(const List<T, LENGTH>& other);
    // 避免指向同一片内存时需要调用copy
    List<T, LENGTH> copy() const;
    List<T, LENGTH>& operator+=(const List<T, LENGTH>& other);

    // List中每个元素自增一个常数
    template <typename Tintput>
    List<T, LENGTH>& operator+=(const Tintput& b);

    template <typename Tintput>
    List<T, LENGTH>& operator-=(const Tintput& b);

    List<T, LENGTH>& operator*=(const List<T, LENGTH>& other);
    List<T, LENGTH>& operator/=(const List<T, LENGTH>& other);

    template <typename Tintput>
    List<T, LENGTH>& operator*=(const Tintput& v0);

    template <typename TT, uint32_t Len>
    friend List<TT, Len> operator*(const List<TT, Len>& other, const int32_t& v0);

    template <typename Tintput, class Type, uint Len>
    friend List<Type, Len> operator*(const Tintput& v0, const List<Type, Len>& other);

    template <typename Tintput>
    List<T, LENGTH>& operator/=(const Tintput& v0);

    List<T, LENGTH>& operator-=(const List<T, LENGTH>& other);

    template <typename Type, uint32_t Len>
    friend const List<Type, Len> operator+(const List<Type, Len>& a, const List<Type, Len>& b);

    template <typename Tintput, typename Type, uint32_t Len>
    friend const List<Type, Len> operator+(const List<Type, Len>& a, const Tintput& b);

    template <typename Tintput, typename Type, uint32_t Len>
    friend const List<T, LENGTH> operator+(const Tintput& b, const List<T, LENGTH>& a);

    template <typename Type, uint32_t Len, class Tintput>
    friend const List<Type, Len> operator-(const List<Type, Len>& a, const Tintput& b);

    template <typename Type, uint32_t Len>
    friend const List<Type, Len> operator-(const List<Type, Len>& a, const List<Type, Len>& b);
    template <class T1, uint32_t Len1, class T2, uint32_t Len2>
    friend List<T2, Len2> ts_prod(const List<T1, Len1>& a, const List<T2, Len2>& b);

   public:
    // 事后处理，用做计算量大优化
    virtual void ExPost() {}

   private:
    uint32_t m_min_length;
    T m_list[LENGTH];
    T* m_itr;
    size_t m_cum_size;
};
template <class T1, uint32_t Len1, class T2, uint32_t Len2>
List<T2, Len2> ts_prod(const List<T1, Len1>& a, const List<T2, Len2>& b);
}  // namespace hft

#include "List-inc.h"
