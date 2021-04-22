#include "hft/math_tool.hpp"
#include <cmath>
#include <unordered_map>
// 函数
#define SIGN(x) ((0 < x) - (x < 0))
#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x > y ? y : x)
/*
 *  get_mean( itr_begin, itr_end)
 *  求[itr_begin, itr_end] 内的均值，包含itr_begin和itr_end
 * */
namespace math_tool {
double entropy_fast(const int* _begin, const int* _end) {
    // 取值只有 0, 1, -1
    int n_0{0}, n_p{0}, n_n{0};
    int tmp{0};
    for (auto itr = _begin; itr <= _end; ++itr) {
        tmp = *itr;
        if (tmp == 0) {
            ++n_0;
        } else if (tmp == 1) {
            ++n_p;
        } else {
            ++n_n;
        }
    }
    double tmp_entropy{0};
    double weight = 1.0 / (_end - _begin + 1);
    if (n_0 != 0) {
        tmp_entropy += n_0 * weight * log(n_0 * weight);
    }
    if (n_n != 0) {
        tmp_entropy += n_n * weight * log(n_n * weight);
    }
    if (n_p != 0) {
        tmp_entropy += n_p * weight * log(n_p * weight);
    }
    return tmp_entropy;
}

}  // namespace math_tool
