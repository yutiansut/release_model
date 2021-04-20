#include "kernel/strategy/common/queue/queue_peak.h"
#include "base/util.h"

using namespace hft;

//////////////////////////////////////////
void QueueSort::Sort(int l, int r) {
    // big -> small
    if (l < r) {
        int idx = Partion(l, r);
        Sort(l, idx-1);
        Sort(idx+1, r);
    }
}
int QueueSort::Partion(int low, int high) {
    double key = sorted[low];
    while (low < high) {
        while (low < high && Compare(key, sorted[high]) <= 0) {
            --high;
        }
        if (low < high) {
            // Swap(sorted[low], sorted[high]);
            sorted[low] = sorted[high];
            ++low;
        }

        while(low < high && Compare(key, sorted[low]) > 0) {
            ++low;
        }
        if (low < high) {
            // Swap(sorted[low], sorted[high]);
            sorted[high] = sorted[low];
            --high;
        }
    }
    sorted[low] = key;
    return low;
}

//////////////////////////////////////////
bool QuesortInsert::SetValue(double v) {
    double first = datas.first();
    int pos = static_cast<int>(datas.pos);
    bool filled = datas.SetValue(v);
    if (datas.count <= datas.size()) {
        len = datas.count;
    }

    if (datas.count > (uint32_t)len) {
        pos = hft::BinarySearchHL(sorted, first, 0, len-1);
        // 多一步检查，防止崩溃
        if (pos >= 0) {
            // sorted[pos] = v;
            // SortInsert(pos);
            SortInsertBinary(pos, v, len);
        //     LOG_ERROR("BinarySearchHL pos=[%d] key=[%g]未找到", pos, first);
        }
    } else {
        sorted[pos] = v;
        SortInsert(pos);
    }

    return filled;
}
double QuesortInsert::Quantile() const {
    if (last_pos == -1)
        return 0.5;
    double tmp = sorted[last_pos];
    int sum_len = 1;
    int sum = last_pos;
    for (int i = last_pos-1; i >= 0; --i) {
        if (Compare(tmp, sorted[i]) == 0) {
            sum_len += 1;
            sum += i;
        } else {
            break;
        }
    }
    for (int i = last_pos+1; i <= this->len; ++i) {
        if (Compare(tmp, sorted[i]) == 0) {
            sum_len += 1;
            sum += i;
        } else {
            break;
        }
    }
    return 1.0 * sum / (sum_len * (this->len - 1));
}
/*
// 600长度几乎没差别
bool QuesortInsert::SetValue2(double v) {
    double first = datas.first();
    int pos = static_cast<int>(datas.pos);
    bool filled = datas.SetValue(v);
    if (datas.count <= datas.size()) {
        len = datas.count;
    }

    if (datas.count > (uint32_t)len) {
        for (int i = 0; i < len; ++i) {
            if (Compare(sorted[i], first) == 0) {
                sorted[i] = v;
                pos = i;
                break;
            }
        }
    } else {
        sorted[pos] = v;
    }
    SortInsert(pos);
    return filled;
}
*/

void QuesortInsert::SortInsert(int pos) {
    bool is_first = true;
    // 判断那部分不用排序
    int last = last_index(pos);
    int next = next_index(pos);
    int cmp1 = Compare(sorted[pos], sorted[last]);
    int cmp2 = Compare(sorted[pos], sorted[next]);
    int begin = 0, end = 0;
    if (pos == 0 || pos == len - 1) {
        begin = 0;
        end = len-1;
    } else if (cmp1 > 0) {
        begin = 0;
        end = pos;
    } else if (cmp2 < 0) {
        begin = pos;
        end = len-1;
    } else {
        return;
    }

    // for (int i = 0; i < len; ++i) {
    for (int i = begin; i <= end; ++i) {
        if (i == pos) {
            continue;
        }
        int cmp = Compare(sorted[pos], sorted[i]);
        if (cmp > 0 && i < pos) {
            Swap(sorted[pos], sorted[i]);
            if (is_first) {
                last_pos = i;
                is_first = false;
            }
        } else if (cmp < 0 && i > pos) {
            Swap(sorted[pos], sorted[i]);
            pos = i;
            if (is_first) {
                last_pos = i;
                is_first = false;
            }
        } else if (cmp > 0 && i > pos) {
            break;
        }
    }
}
void QuesortInsert::SortInsertBinary(int pos, double key, int size) {
    if (size == 0) {
        sorted[0] = key;
        return;
    }
    int low = 0;
    int high = size - 1;
    int mid = 0;
    while (low <= high) {
        mid = (high + low) / 2;
        int cmp = Compare(key, sorted[mid]);
        if (cmp > 0) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    // printf("sort=[%g] range=[%d %d]\n", key, mid, pos);
    if (mid < pos) {
        if (Compare(key, sorted[mid]) < 0) {
            ++mid;
        }
        for (int i = pos; i > mid; --i) {
            sorted[i] = sorted[i-1];
        }
    } else if (mid > pos) {
        if (Compare(key, sorted[mid]) > 0) {
            --mid;
        }
        for (int i = pos; i < mid; ++i) {
            sorted[i] = sorted[i+1];
        }
    }
    sorted[mid] = key;
}

//////////////////////////////////////////
bool QuesortInsopt::SetValue(double v) {
    double first = datas.first();
    // int pos = static_cast<int>(datas.pos);
    bool filled = datas.SetValue(v);
    // if (datas.count <= datas.size()) {
    //     len = datas.count;
    // }

    int pos = 0;
    if (datas.count > datas.size()) {
        pos = BinarySearchHL(sorted, first, 0, len-1);
        if (pos >= 0) {
            sorted[pos].n -= 1;
            if (sorted[pos].n == 0) {
                SortInsert(pos);
                --len;
                // printf("[pos1] len=[%d] minus[%g]\n", len, first);
                // this->print();
            }
        }
    }
    //
    pos = BinarySearchHL(sorted, v, 0, len-1);
    if (pos < 0) {
        sorted[len].v = v;
        sorted[len].n = 1;
        ++len;
        SortInsert(len-1);
        // printf("[pos2] sorted[%d]=[%g(%d)]\n", len-1, sorted[len-1].v, sorted[len-1].n);
    } else {
        sorted[pos].n += 1;
    }

    return filled;
}
void QuesortInsopt::SortInsert(int pos) {
    // 判断那部分不用排序
    int last = last_index(pos);
    int next = next_index(pos);
    int cmp1 = Compare(sorted[pos], sorted[last]);
    int cmp2 = Compare(sorted[pos], sorted[next]);
    int begin = 0, end = 0;
    if (pos == 0 || pos == len - 1) {
        begin = 0;
        end = len-1;
    } else if (cmp1 > 0) {
        begin = 0;
        end = pos;
    } else if (cmp2 < 0) {
        begin = pos;
        end = len-1;
    } else {
        return;
    }

    // for (int i = 0; i < len; ++i) {
    for (int i = begin; i <= end; ++i) {
        if (i == pos) {
            continue;
        }
        int cmp = Compare(sorted[pos], sorted[i]);
        // printf("compare2, pos_i=[%d %d] cmp=[%d]\n", pos, i, cmp);
        if (cmp > 0 && i < pos) {
            Swap(sorted[pos], sorted[i], tmp);
        } else if (cmp < 0 && i > pos) {
            Swap(sorted[pos], sorted[i], tmp);
            pos = i;
        } else if (cmp > 0 && i > pos) {
            break;
        }
    }
}
/*
void QuesortInsopt::Swap(QsiData& d1, QsiData& d2) {
    tmp = std::move(d1);
    d1 = std::move(d2);
    d2 = std::move(tmp);
}
*/
int QuesortInsopt::Compare(QsiData& d1, QsiData& d2) {
    if (d1.n == 0)
        return -1;
    if (d2.n == 0)
        return 1;

    return hft::Compare(d1.v, d2.v);
}
int QuesortInsopt::BinarySearchHL(const std::vector<QsiData> &a, double x, int left, int right) const {
    while (left <= right) {
        int n = left;
        if (left != right) {
            n = (left + right) / 2;
        }
        int cmp = hft::Compare(x, a[n].v);
        if (cmp == 0)
            return n;
        else if (cmp > 0)
            right = n - 1;
        else
            left = n + 1;
    }
    return -1;
}

//////////////////////////////////////////
bool QuesortInsoptWPF::SetValue(double v, int n/*, Fet2Roll& d*/) {
    int pos = BinarySearchHL(sorted, v, 0, len-1);
    if (pos < 0) {
        if (len >= static_cast<int>(sorted.size())) {
            sorted.resize(len + 100);
        }
        sorted[len].v = v;
        sorted[len].n = n;
        // sorted[len].fet2 = std::move(d);
        ++len;
        SortInsert(len-1);
    } else {
        sorted[pos].n = n;
        // sorted[pos].fet2 = std::move(d);
    }
    return true;
}
void QuesortInsoptWPF::SortInsert(int pos) {
    int begin = 0, end = len-1;
    for (int i = begin; i <= end; ++i) {
        if (i == pos) {
            continue;
        }
        int cmp = Compare(sorted[pos], sorted[i]);
        // printf("compare2, pos_i=[%d %d] cmp=[%d]\n", pos, i, cmp);
        if (cmp > 0 && i < pos) {
            Swap(sorted[pos], sorted[i], tmp);
        } else if (cmp < 0 && i > pos) {
            Swap(sorted[pos], sorted[i], tmp);
            pos = i;
        } else if (cmp > 0 && i > pos) {
            break;
        }
    }
}
int QuesortInsoptWPF::BinarySearchHL(const std::vector<QsiData> &a, double x, int left, int right) const {
    while (left <= right) {
        int n = left;
        if (left != right) {
            n = (left + right) / 2;
        }
        int cmp = hft::Compare(x, a[n].v);
        if (cmp == 0)
            return n;
        else if (cmp > 0)
            right = n - 1;
        else
            left = n + 1;
    }
    return -1;
}
