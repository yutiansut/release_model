#ifndef KERNEL_STRATEGY_COMMON_QUEUE_QUEUE_PEAK_H_
#define KERNEL_STRATEGY_COMMON_QUEUE_QUEUE_PEAK_H_

#include <sstream>
#include <vector>
#include "base/base.h"
#include "kernel/strategy/common/queue/queue_roll.hpp"
#include "kernel/strategy/common/struct.h"

namespace hft {
//////////////////////////////////////////
// 用于处理循环队列获取峰值
class QueuePeak {
public:
    inline void Init(uint32_t s) {
        datas.Init(s);
        len = s;
    }
    // 返回队列是否满
    inline bool SetValue(double v) {
        return datas.SetValue(v);
    }

    inline bool IsMax(double v) {
        for (int i = 0; i < len; ++i) {
            double m = datas.results[i];
            if (Compare(v, m) < 0) {
                return false;
            }
        }
        return true;
    }

protected:
    QueueRoll<double> datas;
    int len;
};

//////////////////////////////////////////
// 二分排序
class QueueSort {
public:
    QueueSort() : len(0) {}
    inline void Init(uint32_t s) {
        datas.Init(s);
        sorted.resize(s);
        // len = s;
    }
    virtual bool SetValue(double v) {
        bool filled = datas.SetValue(v);
        if (datas.count <= datas.size()) {
            len = datas.count;
        }
        // 队列满才排序
        if (filled) {
            sorted = datas.results;
            int tail = std::max(len-1, 0);
            Sort(0, tail);
        }
        return filled;
    }

    inline double max() const {
        return sorted[0];
    }
    inline double min() const {
        return sorted[len-1];
    }
    inline bool is_filled() const {
        return datas.is_filled();
    }

    inline void print() {
        for (int i = 0; i < len; ++i) {
            printf("%g ", sorted[i]);
        }
        printf("| len=[%d]\n", len);
    }

protected:
    void Sort(int l, int r);
    int Partion(int low, int high);

    inline void Swap(double& d1, double& d2) {
        if (Compare(d1, d2) == 0) {
            return;
        }
        tmp = std::move(d1);
        d1 = std::move(d2);
        d2 = std::move(tmp);
    }
    inline int next_index(int p) {
        if ((p+1) >= len) {
            return 0;
        } else {
            return p + 1;
        }
    }
    inline int last_index(int p) {
        if ((p-1) < 0) {
            return len - 1;
        } else {
            return p - 1;
        }
    }

protected:
    QueueRoll<double> datas;
    std::vector<double> sorted;     // big -> small排序
    int len;
    double tmp;
};

class QuesortInsert : public QueueSort {
public:
    QuesortInsert() : /*sorted_len(0),*/ last_pos(-1) {}
    bool SetValue(double v) override;
    // bool SetValue2(double v);

    double Quantile() const;
    // 从大到小排序
    void SortInsert(int pos);
    void SortInsertBinary(int pos, double v, int size);

protected:
    // int sorted_len;
    // 最后值插入的有序位置
    int last_pos;
};

//////////////////////////////////////////
// 排序队列(特别优化)
class QuesortInsopt {
struct QsiData {
    double v;
    int n;    // 这里计数用
    QsiData() : v(0), n(0) {}
};

public:
    QuesortInsopt() : len(0) {}
    inline void Init(uint32_t s) {
        datas.Init(s);
        sorted.resize(s);
    }
    inline double max() const {
        return sorted[0].v;
    }
    inline double min() const {
        return sorted[len-1].v;
    }
    inline double diff() const {
        return datas.last() - datas.first();
    }
    inline bool is_filled() const {
        return datas.is_filled();
    }
    inline int Find(double p) const {
        return BinarySearchHL(sorted, p, 0, len-1);
    }
    inline const QsiData& data(int n) const {
        return sorted[n];
    }

    inline void print() {
        for (int i = 0; i < len; ++i) {
            printf("%g(%d) ", sorted[i].v, sorted[i].n);
        }
        printf("| len=[%d]\n", len);
    }
    //
    bool SetValue(double v);
    void SortInsert(int pos);

    inline bool SetLastValue() {
        SetValue(datas.last());
        return datas.is_filled();
    }
    inline int next_index(int p) {
        if ((p+1) >= len) {
            return 0;
        } else {
            return p + 1;
        }
    }
    inline int last_index(int p) {
        if ((p-1) < 0) {
            return len - 1;
        } else {
            return p - 1;
        }
    }

protected:
    // void Swap(QsiData& d1, QsiData& d2);
    int Compare(QsiData& d1, QsiData& d2);
    int BinarySearchHL(const std::vector<QsiData> &a, double x, int left, int right) const;

protected:
    QueueRoll<double> datas;
    std::vector<QsiData> sorted;
    int len;
    QsiData tmp;
};

class QuesortInsoptWPF {
struct QsiData {
    double v;
    int n;    // 这里直接赋值
    // Fet2Roll fet2;
    QsiData() : v(0), n(0) {}
};

public:
    QuesortInsoptWPF() : len(0) {}
    inline void Init(uint32_t s) {
        sorted.resize(s);
    }
    inline int Find(double p) const {
        return BinarySearchHL(sorted, p, 0, len-1);
    }
    inline double max() const {
        return sorted[0].v;
    }
    inline double min() const {
        return sorted[len-1].v;
    }
    inline const QsiData& data(int n) const {
        return sorted[n];
    }

    //
    inline int Compare(QsiData& d1, QsiData& d2) {
        return hft::Compare(d1.v, d2.v);
    }
    inline std::string str() {
        std::ostringstream ss;
        for (int i = 0; i < len; ++i) {
            ss << sorted[i].v << "(" << sorted[i].n << ") ";
            // printf("%g(%d) ", sorted[i].v, sorted[i].n);
        }
        // printf("| len=[%d]\n", len);
        ss << "| len=[" << len << "]";
        return ss.str();
    }

    bool SetValue(double p, int v/*, Fet2Roll& d*/);

protected:
    void SortInsert(int pos);
    int BinarySearchHL(const std::vector<QsiData> &a, double x, int left, int right) const;

protected:
    // big->small排序
    std::vector<QsiData> sorted;
    int len;
    QsiData tmp;
};
}    // namespace hft
#endif    // KERNEL_STRATEGY_COMMON_QUEUE_QUEUE_PEAK_H_
