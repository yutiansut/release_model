/********************************************
* @brief      定义一些定长回环数据结构，方便策略调用和二次开发(非线程安全)
* @date       20190826
* @author     cangmj
********************************************/
#ifndef KERNEL_STRATEGY_COMMON_QUEUE_QUEUE_ROLL_HPP_
#define KERNEL_STRATEGY_COMMON_QUEUE_QUEUE_ROLL_HPP_

#include <vector>
#include "kernel/strategy/common/algo.h"
#include "base/util.h"

namespace hft {
//////////////////////////////////////////
// 定长循环ring数组(非线程安全)
template<typename T>
class QueueRoll {
public:
    QueueRoll() : is_filled_(false), pos(0), count(0) {}

    inline virtual void Init(uint32_t s) {
        results.resize(s);
    }
    // 回调实现(可在这里自定义需要功能)
    virtual void onPopValue(const T& v) {}
    virtual void onSetValue(const T& v) {}

    // true:队列满了
    inline virtual bool SetValue(const T& v) {
        if (is_filled()) {
            // 将会被覆盖, 所以回调pop
            onPopValue(results[pos]);
        }
        results[pos] = v;
        onSetValue(v);
        //
        NextPos();
        return is_filled_;
    }
    //
    inline const T& last() const {
        int p = pos;
        if (p <= 0) {
            return results[results.size()-1];
        }
        return results[p-1];
    }
    /*
    inline const T& pre_last() const {
        if (pos == 0) {
            return results[results.size()-2];
        } else if (pos == 1) {
            return results[results.size()-1];
        }
        return results[pos-2];
    }
    */
    // -1:last -2:pre_last
    inline const T& pre_data(int n = -1) const {
        int p = pos + n;    // n<0
        if (p < 0)
            p = results.size() + p;
        return results[p];
    }
    inline const T& first() const {
        if (count <= results.size())    {
            return results[0];
        } else {
            return results[pos];
        }
    }
    // 获取data数据
    inline const T& data(int i) const {
        return results[i];
    }
    inline T& data(int i) {
        return results[i];
    }
    // 数组长度
    inline size_t size() const {
        return results.size();
    }
    inline bool is_filled() const {
        return is_filled_;
    }

    // 循环用(注意: 非线程安全)
    inline int idx_first() const {
        if (count <= results.size())    {
            return 0;
        } else {
            return pos;
        }
    }
    inline int idx_end() const {
        return -1;
    }
    inline void idx_next(int& cur) {
        ++cur;
        if (cur >= static_cast<int>(results.size())) {
            cur = 0;
        }
        if (UNLIKELY(cur == static_cast<int>(pos))) {
            cur = -1;
        }
    }

protected:
    inline void NextPos() {
        ++count;
        ++pos;
        if (pos == results.size()) {
            if (!is_filled_) {
                is_filled_ = true;
            }
            pos = 0;
        }
    }

public:
    // 数据是否填满
    bool is_filled_;
    uint32_t pos;
    uint32_t count;
    std::vector<T> results;
};

//////////////////////////////////////////
// 可变定长ring数组基类(根据timeval保存固定时间内的数据)
// @note: T数据结构 必须包含timeval tv;变量
template<typename T>
class QueueRollDym {
public:
    QueueRollDym() : is_filled_(false), head(0), tail(0), cnt(0), keep_usec(0) {}

    inline virtual void Init(uint32_t s, uint32_t keep_sec) {
        size = s;
        results.resize(s);
        keep_usec = keep_sec * 1000000;
    }
    // 回调实现
    virtual void onSetValue(const T& v) {}
    virtual void onMoveHeadToNext(const T& v) {}

    // 写入数据
    inline bool SetValue(const T& v) {
        results[tail] = std::move(const_cast<T&>(v));
        onSetValue(v);
        NextTail();
        //
        if (UNLIKELY(cnt >= size)) {
            LOG_ERROR("[QueueRollDym] count=[%u]超过队列长度[%zu]", cnt, results.size());
            return false;
        }
        return true;
    }
    // 移动head指针到下一个时间戳范围
    inline void MoveHeadToNext() {
        T& dat = results[last_tail()];
        while (cnt > 0) {
            T& dah = results[head];
            if (TimevalDiff(dat.tv, dah.tv) >= keep_usec) {
                onMoveHeadToNext(dah);
                NextHead();
            } else {
                break;
            }
        }
    }
    //
    inline uint32_t last_tail() const {
        if (UNLIKELY(tail == 0))
            return results.size() - 1;
        return tail - 1;
    }
    inline const T& last() const {
        return results[last_tail()];
    }
    inline const T& first() const {
        return results[head];
    }
    // 获取data数据
    inline const T& data(int i) const {
        return results[i];
    }
    inline T& data(int i) {
        return results[i];
    }
    // 数据个数
    inline size_t count() const {
        return cnt;
    }
    // 是否保留时间的段数据填充
    inline bool is_filled() const {
        return is_filled_;
    }

    // 循环用(注意: 非线程安全)
    inline int idx_first() const {
        return head;
    }
    inline int idx_end() const {
        return -1;
    }
    inline void idx_next(int& cur) {
        ++cur;
        if (cur >= static_cast<int>(results.size())) {
            cur = 0;
        }
        if (UNLIKELY(cur == static_cast<int>(tail))) {
            cur = -1;
        }
    }

protected:
    inline void NextHead() {
        --cnt;
        ++head;
        if (UNLIKELY(head >= size)) {
            head = 0;
        }
    }
    inline void NextTail() {
        ++cnt;
        ++tail;
        if (UNLIKELY(tail >= size)) {
            tail = 0;
        }
        if (UNLIKELY(!is_filled_)) {
            T& dat = results[last_tail()];
            T& dah = results[head];
            if (TimevalDiff(dat.tv, dah.tv) >= keep_usec) {
                is_filled_ = true;
            }
        }
    }

public:
    // 数据是否填满
    bool is_filled_;
    int head;
    int tail;
    int cnt;
    int size;
    int64_t keep_usec;
    std::vector<T> results;
};

//////////////////////////////////////////
template<typename T>
class QueavgRoll : public QueueRoll<T> {
    typedef QueueRoll<T> qr;

public:
    QueavgRoll() : result_sum(0) {}

    inline void Init(uint32_t s) override {
        qr::results.resize(s);
        avg_multi = 1.0 / s;
    }
    //
    void onPopValue(const T& v) override {
        result_sum -= v;
    }
    void onSetValue(const T& v) override {
        result_sum += v;
    }
    /*
    inline bool SetValue(const T& v) override {
        if (qr::is_filled()) {
            // 将会被覆盖，所以减去
            result_sum -= qr::results[qr::pos];
        }
        qr::results[qr::pos] = v;
        result_sum += v;
        //
        qr::NextPos();
        return qr::is_filled();
    }
    */
    inline bool SetLastValue() {
        qr::SetValue(qr::last());
        return qr::is_filled();
    }

    inline T summary() const {
        return result_sum;
    }
    inline double average() const {
        return result_sum * avg_multi;
    }

public:
    // 平均值乘数, = 1/len
    double avg_multi;
    T result_sum;
};

//////////////////////////////////////////
// skewness
class QueskewRoll {
public:
    QueskewRoll() {}

    inline void Init(uint32_t s) {
        v1.Init(s);
        v2.Init(s);
    }
    // true:队列满了
    inline bool SetValue(double d) {
        double tmp = d * d;
        v1.SetValue(tmp * d);
        v2.SetValue(tmp);
        return is_filled();
    }
    inline bool SetValue(double d1, double d2) {
        v1.SetValue(d1);
        v2.SetValue(d2);
        return is_filled();
    }

    inline bool SetLastValue() {
        v1.SetLastValue();
        v2.SetLastValue();
        return is_filled();
    }

    // return skewness
    inline double value() const {
        double a = Math::fasqrt(v1.size()) * v1.summary();
        double b = fabs(v2.summary());
        b = Math::fasqrt(b * b * b);
        return Compare(b, 0) == 0 ? 0 : (a / b);
    }

    inline bool is_filled() const {
        return v1.is_filled();
    }

public:
    QueavgRoll<double> v1;
    QueavgRoll<double> v2;
};
// kurtosis
class QuekurtoRoll {
public:
    QuekurtoRoll() {}

    inline void Init(uint32_t s) {
        v1.Init(s);
        v2.Init(s);
    }
    // true:队列满了
    inline bool SetValue(double d) {
        double tmp = d * d;
        v1.SetValue(tmp * tmp);
        v2.SetValue(tmp);
        return is_filled();
    }
    inline bool SetValue(double d1, double d2) {
        v1.SetValue(d1);
        v2.SetValue(d2);
        return is_filled();
    }
    inline bool SetLastValue() {
        v1.SetLastValue();
        v2.SetLastValue();
        return is_filled();
    }

    // return kurtosis
    inline double value() const {
        double b = v2.average();
        b = b * b;
        return Compare(b, 0) == 0 ? 0 : (v1.average() / b - 3);
    }

    inline bool is_filled() const {
        return v1.is_filled();
    }

public:
    QueavgRoll<double> v1;
    QueavgRoll<double> v2;
};
// stdev
class QuestdevRoll {
public:
    QuestdevRoll() {}

    inline void Init(uint32_t s) {
        v1.Init(s);
        v2.Init(s);
    }
    // true:队列满了
    inline bool SetValue(double d) {
        v1.SetValue(d);
        v2.SetValue(d * d);
        return is_filled();
    }
    inline bool SetValue(double d1, double d2) {
        v1.SetValue(d1);
        v2.SetValue(d2);
        return is_filled();
    }
    inline bool SetLastValue() {
        v1.SetLastValue();
        v2.SetLastValue();
        return is_filled();
    }

    // return stdev
    inline double value() const {
        double v = (v2.summary() - v1.summary() * v1.summary() / v1.size());
        return (v1.size() - 1) == 0 ? 0 : Math::fasqrt(v / (v1.size() - 1));
    }
    // return var
    inline double var() const {
        double v = (v2.summary() - v1.summary() * v1.summary() / v1.size());
        // LOG_INFO("var: v1.sum=%g v2.sum=%g len=%zu", v1.summary(), v2.summary(), v1.size());
        return (v1.size() - 1) == 0 ? 0 : (v / (v1.size() - 1));
    }

    inline bool is_filled() const {
        return v1.is_filled();
    }

public:
    QueavgRoll<double> v1;
    QueavgRoll<double> v2;
};
// correlation
class QuecorrRoll {
public:
    QuecorrRoll() {}

    inline void Init(uint32_t s) {
        va.Init(s);
        va2.Init(s);
        vb.Init(s);
        vb2.Init(s);
        vab.Init(s);
        len = s;
    }
    // true:队列满了
    inline bool SetValue(double a, double a2, double b, double b2, double ab) {
        va.SetValue(a);
        va2.SetValue(a2);
        vb.SetValue(b);
        vb2.SetValue(b2);
        vab.SetValue(ab);
        return is_filled();
    }
    inline bool SetLastValue() {
        va.SetLastValue();
        va2.SetLastValue();
        vb.SetLastValue();
        vb2.SetLastValue();
        vab.SetLastValue();
        return is_filled();
    }

    inline double value() const {
        double v = Math::fasqrt((len * va2.summary() - va.summary() * va.summary()) * (len * vb2.summary() - vb.summary() * vb.summary()));
        // LOG_INFO("[corr] a=[%g %g] b=[%g %g] ab=[%g] len=[%d]", va.summary(), va2.summary(), vb.summary(), vb2.summary(), vab.summary(), len);
        return Compare(v, 0) == 0 ? 0 : (len * vab.summary() - va.summary() * vb.summary()) / v;
    }
    inline bool is_filled() const {
        return va.is_filled();
    }

public:
    QueavgRoll<double> va;
    QueavgRoll<double> va2;
    QueavgRoll<double> vb;
    QueavgRoll<double> vb2;
    QueavgRoll<double> vab;
    uint32_t len;
};

//////////////////////////////////////////
class QuevolsqrtRoll {
public:
    QuevolsqrtRoll() {}

    inline void Init(uint32_t s) {
        v1.Init(s);
        v2.Init(s);
    }
    // true:队列满了
    inline bool SetValue(double d1, double d2) {
        v1.SetValue(d1);
        v2.SetValue(d2);
        return is_filled();
    }
    inline bool SetLastValue() {
        v1.SetLastValue();
        v2.SetLastValue();
        return is_filled();
    }

    inline double value() const {
        // double v = v1.summary() / v2.summary();
        double v = v2.summary();
        return Compare(v, 0) == 0 ? 0 : (v1.summary() / v);
    }
    inline bool is_filled() const {
        return v1.is_filled();
    }

public:
    QueavgRoll<double> v1;
    QueavgRoll<double> v2;
};

// sum(d1) / sqrt(sum(d2))
class QuePersqrtVolRoll {
public:
    QuePersqrtVolRoll() {}

    inline void Init(uint32_t s) {
        v1.Init(s);
        v2.Init(s);
    }
    // true:队列满了
    inline bool SetValue(double d1, double d2) {
        v1.SetValue(d1);
        v2.SetValue(d2);
        return is_filled();
    }
    inline bool SetLastValue() {
        v1.SetLastValue();
        v2.SetLastValue();
        return is_filled();
    }

    inline double value() const {
        double v = v2.summary();
        return Compare(v, 0) == 0 ? 0 : v1.summary() / Math::fasqrt(v);
    }
    inline bool is_filled() const {
        return v1.is_filled();
    }

public:
    QueavgRoll<double> v1;
    QueavgRoll<double> v2;
};
}    // namespace hft
#endif    // KERNEL_STRATEGY_COMMON_QUEUE_QUEUE_ROLL_HPP_
