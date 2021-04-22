#pragma once
#include <chrono>

#define NOW() std::chrono::steady_clock::now()
#define DURATION_ns(tag_start, tag_end)                                                                                \
    std::chrono::duration_cast<std::chrono::nanoseconds>(tag_end - tag_start).count()
