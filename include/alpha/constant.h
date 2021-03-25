#pragma once

#include <cstdint>
#include <string>
namespace husky {
namespace oms {
const char ORDER_INACTIVE = 'I';
const char ORDER_DISCARD = 'S';
const char ORDER_TO_ACTIVATE = 'W';
const char ORDER_PARKED = 'P';
const char ORDER_ACTIVE = 'A';
const char ORDER_MARKET_ACTIVE = 'C';
const char ORDER_MARKET_ACCEPT = 'O';
const char ORDER_REJECT = 'E';
const char ORDER_MARKET_REJECT = 'M';
const char ORDER_FILLED = 'F';
const char ORDER_TO_CANCEL = 'X';
const char ORDER_CANCELED = 'D';
// const char ORDER_CXL_REJECT = 'R'; 和ORDER_FILLED冲突

inline static std::string order_status_str(const char c) {
    switch (c) {
        case ORDER_INACTIVE:
            return "INACTIVE";
        case ORDER_DISCARD:
            return "DISCARD";
        case ORDER_TO_ACTIVATE:
            return "TO_ACTIVE";
        case ORDER_ACTIVE:
            return "ACTIVE";
        case ORDER_MARKET_ACCEPT:
            return "MARKET_ACCEPT";
        case ORDER_REJECT:
            return "REJECT";
        case ORDER_MARKET_REJECT:
            return "MARKET_REJECT";
        case ORDER_FILLED:
            return "FILLED";
        case ORDER_TO_CANCEL:
            return "TO_CANCEL";
        case ORDER_CANCELED:
            return "CANCELED";
        default:
            return "";
    }
}

typedef char Side;
const Side SideBuy = 1;
const Side SideSell = -1;
const Side SideNone = 0;

enum class SideE { Buy, Sell };
enum class POS { LONG, SHORT };

typedef char Direction;
const Direction DirectionLong = 1;
const Direction DirectionShort = -1;

typedef unsigned char Offset;
const Offset OffsetOpen = 0;
const Offset OffsetClose = 1;
const Offset OffsetCloseOvn = 2;

typedef uint8_t ExchangeMask;
const ExchangeMask Q_UNKNOWN = 1 << 0;  // 00000001
const ExchangeMask Q_SHFE = 1 << 1;     // 00000010
const ExchangeMask Q_CFFEX = 1 << 2;    // 00000100
const ExchangeMask Q_CZCE = 1 << 3;     // 00001000
const ExchangeMask Q_DCE = 1 << 4;      // 00010000
const ExchangeMask Q_INE = 1 << 5;      // 00100000

typedef uint8_t OrderType;
const OrderType Q_Default = 0;
const OrderType Q_Normal = 1;  // 普通单
const OrderType Q_Parked = 2;  // 预埋单
const OrderType Q_Exec = 3;    // 执行宣告

inline static ExchangeMask str2ExchangeMask(const std::string& exchangeStr) {
    if (exchangeStr == "SHFE") {
        return Q_SHFE;
    } else if (exchangeStr == "CFFEX") {
        return Q_CFFEX;
    } else if (exchangeStr == "CZCE") {
        return Q_CZCE;
    } else if (exchangeStr == "DCE") {
        return Q_DCE;
    } else if (exchangeStr == "INE") {
        return Q_INE;
    } else {
        return Q_UNKNOWN;
    }
}

/**
 * 结束状态: cancel_success, cancel_fail/filled filled
 *
 * 成功撤单 send -> accepted -> canceling,[partial_fill x N] -> cancel_success|(cancel_failed AND filled) -> END
 * 全部成交 send -> accepted -> [partial_filled x N] -> filled -> END
 * 提交失败 send -> rejected -> END
 */
struct single_order {
    char contract[16];     // contract name
    double price;          // order price
    int vol;               // order qty
    int fill;              // <=0 means filled
    long long int oid;     // official order id
    char who[5];           // alpha id
    oms::Side side;        // buy or sell
    oms::Offset offset;    // open or close
    char status;           // order status
    uint64_t start_time;   // signal time
    uint64_t send_time;    // order send time
    uint64_t end_time;     // order end time
    int32_t user_define1;  //
    int32_t user_define2;  //

    void setQty(int qty) {
        this->vol = qty;
        this->fill = this->vol;
    }

    bool isExecuted() const {
        return (this->status == ORDER_FILLED) || (this->status == ORDER_CANCELED && this->fill < this->vol);
    }
};

struct DetailOrder : public single_order {
    ExchangeMask exchange_mask;
    OrderType orderType;
    unsigned long long nSysId;
    char sysId[21];
    //        uint32_t                ii;
    uint32_t si;

    const std::string getExchangeStr() const {
        if (exchange_mask == Q_SHFE) {
            return "SHFE";
        } else if (exchange_mask == Q_CFFEX) {
            return "CFFEX";
        } else if (exchange_mask == Q_CZCE) {
            return "CZCE";
        } else if (exchange_mask == Q_DCE) {
            return "DCE";
        } else if (exchange_mask == Q_INE) {
            return "INE";
        } else {
            return "";
        }
    }
};

struct Signal {
    Side sigSide;
    uint32_t ii;
    Signal() : ii(UINT32_MAX) {}
    Signal(Side side, uint32_t ii) : sigSide(side), ii(ii) {}
    bool isReady() { return ii != UINT32_MAX; }
    void release() { ii = UINT32_MAX; }
};

struct Signal2 : public Signal {
    double suggestPrice;
    int qty;
    Signal2() : Signal() {}
    Signal2(Side side, uint32_t ii, double price, int qty) : Signal(side, ii), suggestPrice(price), qty(qty) {}
};
}  // namespace oms
}  // namespace husky
