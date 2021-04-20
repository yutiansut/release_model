#pragma once
#include "common/log.h"

#define STD_LOG(format, ...) HUSKY_TRADE_INFO(format, ##__VA_ARGS__)
//数字越大， 级别越高，
// 级别控制
#ifdef LOG_LEVEL_1
#define LOG_LEVEL_2
#endif
#ifdef LOG_LEVEL_2
#define LOG_LEVEL_3
#endif
#ifdef LOG_LEVEL_3
#define LOG_LEVEL_4
#endif
#ifdef LOG_LEVEL_4
#define LOG_LEVEL_5
#endif

#ifdef LOG_LEVEL_5
#define LOG_INFO5(format, ...) STD_LOG(format, ##__VA_ARGS__)
#else
#define LOG_INFO5(format, ...)
#endif

#ifdef LOG_LEVEL_4
#define LOG_INFO4(format, ...) STD_LOG(format, ##__VA_ARGS__)
#else
#define LOG_INFO4(format, ...)
#endif

#ifdef LOG_LEVEL_3
#define LOG_INFO3(format, ...) STD_LOG(format, ##__VA_ARGS__)
#else
#define LOG_INFO3(format, ...)
#endif

#ifdef LOG_LEVEL_2
#define LOG_INFO2(format, ...) STD_LOG(format, ##__VA_ARGS__)
#else
#define LOG_INFO2(format, ...)
#endif

#ifdef LOG_LEVEL_1
#define LOG_INFO1(format, ...) STD_LOG(format, ##__VA_ARGS__)
#else
#define LOG_INFO1(format, ...)
#endif

#ifdef HFT_DEBUG
#define LOG_INFO(format, ...) STD_LOG(format, ##__VA_ARGS__)
#else
#define LOG_INFO(format, ...)
#endif
