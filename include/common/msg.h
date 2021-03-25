#pragma once

#include <cstdint>
/*
 * 消息类型定义、枚举
 *
 */
namespace msg {
    typedef unsigned char MSG_TYPE;
    const MSG_TYPE INSTRUCTION = 1;
    const MSG_TYPE ORDER_RECEIPT = 2;
    const MSG_TYPE QUERY_RESULT = 3;

    namespace instruction {
        enum {
            CLOSE_FIN = 2001,      // 平仓并终止
            KEEP_FIN = 2002,       // 持仓并终止
            REFRESH_CONFIG = 2003, // 刷新配置文件
            SYNC_POSITION = 2004,  // 与柜台同步仓位
            RESET_POSITION = 2005,  // 重置仓位为0
            ELEGANT_CLOSE_FIN = 2006 // 优雅平仓
        };
    }

    namespace order {
        typedef char ERROR_CODE;
        const ERROR_CODE INSUFFICIENT_FUND = 1;
        const ERROR_CODE INSUFFICIENT_POSITION = 2;
        const ERROR_CODE ALREADY_FILLED = 3;
        const ERROR_CODE PRICE_RANGE_EXCEED = 4;
        const ERROR_CODE QTY_ILLEGAL = 5;

        enum {
            API_ACCEPT = 1001,
            MARKET_ACCEPT = 1002,
            API_REJECT = 1003,
            MARKET_REJECT = 1004,
            EXEC = 1005,
            CXL = 1006,
            CXL_REJECT = 1007
        };
    }

    namespace response {
        enum {
            ON_QUERY_POSITION = 3001,
            ON_PONG = 3002,
            ON_QUERY_TRADE_FEE = 3003
        };
    }

    namespace order_manager {

    }

    namespace risk_manager {

    }


    struct msg_hdr {
        MSG_TYPE msg_type;
        uint16_t event_type;
    };


    struct msg_frame {
        msg_hdr header;
        union {
            struct {
                int code;
                int user_define_int1;
                int user_define_int2;
                int user_define_int3;
                char message[20];
            } pong;

            struct {
                int ovn_qty;      // 隔夜仓
                int today_qty;    // 今仓
                double  ovn_margin;   // 隔夜保证金
                double  today_margin; // 今仓保证金
                char symbol[16];      // 仓位对应的合约
                int8_t direction;    // -1空, 1多
            } position_info;

            struct {
                int long_position;
                int short_position;
                int ovn_long_position;
                int ovn_short_position;
                double long_open_cost;
                double short_open_cost;
                double position_profit;
                char symbol[16];
            } investor_position;

            struct {
                char exchange[8];
                char symbol[16];
                double upper_limit_price;
                double lower_limit_price;
            } price_limit;

            struct {
                double open_ratio_by_money;
                double open_ratio_by_volume;
                double close_yesterday_ratio_by_money;
                double close_yesterday_ratio_by_volume;
                double close_today_ratio_by_money;
                double close_today_ratio_by_volume;
            } trade_fee;

            struct {
                //char symbol[20];
                int cid;
                double price;
                int qty;
                long long int oid;
                char custom_field_1[8];
                char custom_field_2[8];
            } api_accept;

            struct  {
                int cid;
            } market_accept;

            struct {
                int cid;
                order::ERROR_CODE error_code;
            } api_reject;

            struct {
                int cid;
                char error_text[100];
            } market_reject;

            struct {
                int cid;
                int qty;
                double price; // 实际成交价格
                char eid[25];
            } exec;

            struct {
                int cid;
                int qty;
            } cxl;

            struct {
                int cid;
                order::ERROR_CODE error_code;
            } cxl_reject;
        };
    };
}