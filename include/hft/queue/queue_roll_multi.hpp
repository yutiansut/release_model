#ifndef KERNEL_STRATEGY_COMMON_QUEUE_QUEUE_ROLL_MULTI_HPP_
#define KERNEL_STRATEGY_COMMON_QUEUE_QUEUE_ROLL_MULTI_HPP_

#include "kernel/strategy/common/queue/queue_roll.hpp"
#include "kernel/strategy/common/queue/queue_peak.h"

namespace hft {
//////////////////////////////////////////
template<typename T>
class QueueRollMulti {
public:
    inline void Init(const std::vector<int>& len) {
        roll_data.resize(len.size());
        roll_len = len;
        max_len = 0;
        for (uint32_t i = 0; i < roll_len.size(); ++i) {
            roll_data[i].Init(roll_len[i]);
            if (roll_len[i] > max_len) {
                max_len = roll_len[i];
                max_idx = i;
            }
        }
    }
    inline void Init(int* l, int len) {
        roll_data.resize(len);
        roll_len.resize(len);
        max_len = 0;
        for (uint32_t i = 0; i < roll_len.size(); ++i) {
            roll_len[i] = l[i];
            roll_data[i].Init(roll_len[i]);
            if (roll_len[i] > max_len) {
                max_len = roll_len[i];
                max_idx = i;
            }
        }
    }

    inline bool SetLastValue() {
        for (uint32_t i = 0; i < roll_data.size(); ++i) {
            roll_data[i].SetLastValue();
        }
        return roll_data[max_idx].is_filled();
    }
    inline uint32_t len() const {
        return roll_data.size();
    }
    inline bool is_filled(int idx) const {
        return roll_data[idx].is_filled();
    }
    inline const T& data(int idx) const {
        return roll_data[idx];
    }

    // virtual bool SetValue(double v) { return true; }
    virtual double value(int i) const = 0;

public:
    std::vector<T> roll_data;
    std::vector<int> roll_len;
    int max_len;
    int max_idx;
};

//////////////////////////////////////////
class QueavgRollMulti : public QueueRollMulti<QueavgRoll<double>> {
    typedef QueueRollMulti<QueavgRoll<double>> qr;

public:
    inline bool SetValue(double v) {
        for (uint32_t i = 0; i < roll_data.size(); ++i) {
            roll_data[i].SetValue(v);
        }
        return roll_data[max_idx].is_filled();
    }

    inline double value(int i) const override {
        return roll_data[i].average();
    }
    inline double average(int i) const {
        return roll_data[i].average();
    }
    inline double value_lfdiv(int i) const {
        return Compare(roll_data[i].last(), 0) == 0 ? 0 : (roll_data[i].first() / roll_data[i].last());
    }
};
class QueavgIntRollMulti : public QueueRollMulti<QueavgRoll<int>> {
    typedef QueueRollMulti<QueavgRoll<int>> qr;

public:
    inline bool SetValue(int v) {
        for (uint32_t i = 0; i < roll_data.size(); ++i) {
            roll_data[i].SetValue(v);
        }
        return roll_data[max_idx].is_filled();
    }

    inline double value(int i) const override {
        return roll_data[i].average();
    }
    inline double average(int i) const {
        return roll_data[i].average();
    }
    inline double summary(int i) const {
        return roll_data[i].summary();
    }
};

class QueskewRollMulti : public QueueRollMulti<QueskewRoll> {
    typedef QueueRollMulti<QueskewRoll> qr;

public:
    inline bool SetValue(double v) {
        double tmp = v * v;
        for (uint32_t i = 0; i < roll_data.size(); ++i) {
            roll_data[i].SetValue(tmp*v, tmp);
        }
        return roll_data[max_idx].is_filled();
    }

    double value(int i) const override {
        return roll_data[i].value();
    }
};

class QuekurtoRollMulti : public QueueRollMulti<QuekurtoRoll> {
    typedef QueueRollMulti<QuekurtoRoll> qr;

public:
    inline bool SetValue(double v) {
        double tmp1 = v * v;
        double tmp2 = tmp1 * v * v;
        for (uint32_t i = 0; i < roll_data.size(); ++i) {
            qr::roll_data[i].SetValue(tmp2, tmp1);
        }
        return qr::roll_data[max_idx].is_filled();
    }

    double value(int i) const override {
        return qr::roll_data[i].value();
    }
};

class QuestdevRollMulti : public QueueRollMulti<QuestdevRoll> {
    typedef QueueRollMulti<QuestdevRoll> qr;

public:
    inline bool SetValue(double v) {
        for (uint32_t i = 0; i < roll_data.size(); ++i) {
            roll_data[i].SetValue(v);
        }
        return roll_data[max_idx].is_filled();
    }
    // stdev
    inline double value(int i) const override {
        return roll_data[i].value();
    }
    inline double stdev(int i) const {
        return roll_data[i].value();
    }
    // var
    inline double var(int i) const {
        return roll_data[i].var();
    }
};

//////////////////////////////////////////
class QuevolsqrtRollMulti : public QueueRollMulti<QuevolsqrtRoll> {
    typedef QueueRollMulti<QuevolsqrtRoll> qr;

public:
    inline bool SetValue(double v1, double v2) {
        for (uint32_t i = 0; i < roll_data.size(); ++i) {
            roll_data[i].SetValue(v1, v2);
        }
        return roll_data[max_idx].is_filled();
    }
    // v1.sum() / v2.sum()
    double value(int i) const override {
        return roll_data[i].value();
    }
};

class QuePersqrtVolRollMulti : public QueueRollMulti<QuePersqrtVolRoll> {
    typedef QueueRollMulti<QuePersqrtVolRoll> qr;

public:
    inline bool SetValue(double v1, double v2) {
        for (uint32_t i = 0; i < roll_data.size(); ++i) {
            roll_data[i].SetValue(v1, v2);
        }
        return roll_data[max_idx].is_filled();
    }

    double value(int i) const override {
        return roll_data[i].value();
    }
};
//////////////////////////////////////////
class QuesortInsertMulti : public QueueRollMulti<QuesortInsert> {
    typedef QueueRollMulti<QuesortInsert> qr;

public:
    inline bool SetValue(double v) {
        for (uint32_t i = 0; i < roll_data.size(); ++i) {
            roll_data[i].SetValue(v);
        }
        return roll_data[max_idx].is_filled();
    }
    double value(int i) const override {
        return 0;
    }

    inline double Quantile(int i) const {
        return roll_data[i].Quantile();
    }
    //
    inline double max(int i) const {
        return roll_data[i].max();
    }
    inline double min(int i) const {
        return roll_data[i].min();
    }
};

class QuesortInsoptMulti : public QueueRollMulti<QuesortInsopt> {
    typedef QueueRollMulti<QuesortInsopt> qr;

public:
    inline bool SetValue(double v) {
        for (uint32_t i = 0; i < roll_data.size(); ++i) {
            roll_data[i].SetValue(v);
        }
        return roll_data[max_idx].is_filled();
    }
    double value(int i) const override {
        return 0;
    }

    inline double max(int i) const {
        return roll_data[i].max();
    }
    inline double min(int i) const {
        return roll_data[i].min();
    }
    inline double diff(int i) const {
        return roll_data[i].diff();
    }
};

//////////////////////////////////////////
// 独立的multi，需要做自定义操作
class QuecorrRollMulti {
public:
    inline void Init(int* l, int len) {
        roll_data.resize(len);
        roll_len.resize(len);
        max_len = 0;
        for (uint32_t i = 0; i < roll_len.size(); ++i) {
            roll_len[i] = l[i];
            roll_data[i].Init(roll_len[i]);
            if (roll_len[i] > max_len) {
                max_len = roll_len[i];
                max_idx = i;
            }
        }
    }

    inline bool SetValue(int idx, double a, double a2, double b, double b2, double ab) {
        roll_data[idx].SetValue(a, a2, b, b2, ab);
        return roll_data[idx].is_filled();
    }
    inline bool SetLastValue() {
        for (uint32_t i = 0; i < roll_data.size(); ++i) {
            roll_data[i].SetLastValue();
        }
        return roll_data[max_idx].is_filled();
    }
    inline double value(int i) const {
        return roll_data[i].value();
    }

    inline uint32_t len() const {
        return roll_data.size();
    }
    inline bool is_filled(int idx) const {
        return roll_data[idx].is_filled();
    }

public:
    std::vector<QuecorrRoll> roll_data;
    std::vector<int> roll_len;
    int max_len;
    int max_idx;
};

}    // namespace hft
#endif    // KERNEL_STRATEGY_COMMON_QUEUE_QUEUE_ROLL_MULTI_HPP_
