#pragma once

#include <chrono>
#include <stdio.h>

namespace husky {
    namespace benchmark {
        static std::chrono::steady_clock::time_point t1;
        static std::chrono::steady_clock::time_point t2;
        static inline void bench_start() {
            t1 = std::chrono::steady_clock::now();
        }
        static inline void bench_end() {
            t2 = std::chrono::steady_clock::now();
        }
        static inline int64_t get_duration_nano(size_t iter_num) {
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
            return time_span.count() *  1000000000 / iter_num;
        }
        static inline void get_result(size_t iter_num) {
            printf("avg: %li ns\n", get_duration_nano(iter_num));
        }
    }
}
