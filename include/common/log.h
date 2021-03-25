#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <common/cfg.h>

namespace husky {

    class logger {
    public:
        static void init(const cfg& config);
        static void init(const std::string& logpath);
        inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& getAlphaLogger() { return s_AlphaLogger; }
        inline static std::shared_ptr<spdlog::logger>& getPositionLogger() { return s_PositionLogger; }
        inline static std::shared_ptr<spdlog::logger>& getTradeLogger() { return s_TradeLogger; }
        inline static std::shared_ptr<spdlog::logger>& getMdLogger() { return s_MdLogger; }
        inline static std::shared_ptr<spdlog::logger>& getStdLogger() { return s_STDLogger; }
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_AlphaLogger;
        static std::shared_ptr<spdlog::logger> s_PositionLogger;
        static std::shared_ptr<spdlog::logger> s_TradeLogger;
        static std::shared_ptr<spdlog::logger> s_MdLogger;
        static std::shared_ptr<spdlog::logger> s_STDLogger;
    };

}

#define HUSKY_CORE_TRACE(...)      ::husky::logger::getCoreLogger()->trace(__VA_ARGS__)
#define HUSKY_CORE_INFO(...)       ::husky::logger::getCoreLogger()->info(__VA_ARGS__)
#define HUSKY_CORE_WARN(...)       ::husky::logger::getCoreLogger()->warn(__VA_ARGS__)
#define HUSKY_CORE_ERROR(...)      ::husky::logger::getCoreLogger()->error(__VA_ARGS__)
#define HUSKY_CORE_FATAL(...)      ::husky::logger::getCoreLogger()->critical(__VA_ARGS__)

#define HUSKY_ALPHA_TRACE(...)      ::husky::logger::getAlphaLogger()->trace(__VA_ARGS__)
#define HUSKY_ALPHA_INFO(...)       ::husky::logger::getAlphaLogger()->info(__VA_ARGS__)
#define HUSKY_ALPHA_WARN(...)       ::husky::logger::getAlphaLogger()->warn(__VA_ARGS__)
#define HUSKY_ALPHA_ERROR(...)      ::husky::logger::getAlphaLogger()->error(__VA_ARGS__)
#define HUSKY_ALPHA_FATAL(...)      ::husky::logger::getAlphaLogger()->critical(__VA_ARGS__)

#define HUSKY_POS_TRACE(...)      ::husky::logger::getPositionLogger()->trace(__VA_ARGS__)
#define HUSKY_POS_INFO(...)       ::husky::logger::getPositionLogger()->info(__VA_ARGS__)
#define HUSKY_POS_WARN(...)       ::husky::logger::getPositionLogger()->warn(__VA_ARGS__)
#define HUSKY_POS_ERROR(...)      ::husky::logger::getPositionLogger()->error(__VA_ARGS__)
#define HUSKY_POS_FATAL(...)      ::husky::logger::getPositionLogger()->critical(__VA_ARGS__)

#define HUSKY_TRADE_TRACE(...)      ::husky::logger::getTradeLogger()->trace(__VA_ARGS__)
#define HUSKY_TRADE_INFO(...)       ::husky::logger::getTradeLogger()->info(__VA_ARGS__)
#define HUSKY_TRADE_WARN(...)       ::husky::logger::getTradeLogger()->warn(__VA_ARGS__)
#define HUSKY_TRADE_ERROR(...)      ::husky::logger::getTradeLogger()->error(__VA_ARGS__)
#define HUSKY_TRADE_FATAL(...)      ::husky::logger::getTradeLogger()->critical(__VA_ARGS__)

#define HUSKY_MD_TRACE(...)      ::husky::logger::getMdLogger()->trace(__VA_ARGS__)
#define HUSKY_MD_INFO(...)       ::husky::logger::getMdLogger()->info(__VA_ARGS__)
#define HUSKY_MD_WARN(...)       ::husky::logger::getMdLogger()->warn(__VA_ARGS__)
#define HUSKY_MD_ERROR(...)      ::husky::logger::getMdLogger()->error(__VA_ARGS__)
#define HUSKY_MD_FATAL(...)      ::husky::logger::getMdLogger()->critical(__VA_ARGS__)

#define STDOUT_INFO(...)            ::husky::logger::getStdLogger()->info(__VA_ARGS__)
// release dist ignore trace log
//#if defined(RELEASE) || !defined(TRACE)
//#define HUSKY_CORE_TRACE
//#define HUSKY_ALPHA_TRACE
//#endif