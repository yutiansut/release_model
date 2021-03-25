#pragma once

#include <iomanip>
#include <iostream>

struct md {
    struct l2 {
        double      lastprice;
        double      lastamount;
        double      lastopenint;
        double      bid[5];
        double      ask[5];
        int32_t     bidv[5];
        int32_t     askv[5];
        int32_t     lastvolume;
        uint32_t    update_time;
        int32_t     update_milli_sec;
        uint64_t    nano;

        void output(std::ostream& stream = std::cout, int levelN = 5) {
            stream << std::setprecision(4) << std::fixed << update_time << "," << update_milli_sec << "," << lastprice << "," << lastamount << "," << lastopenint << ",";
            for (int i = 0; i < levelN; i++) {
                stream << bid[i] << "," << ask[i] << "," << bidv[i] << "," << askv[i] << ",";
            }
            stream << lastvolume << "," << nano << std::endl;
        }
    };

    struct TpTv {
        double tp1, tp2, tp3;
        uint32_t tv1, tv2, tv3;
    };

    struct TradeTime {
        uint32_t dayTime1;
        uint32_t dayTime2;
        uint32_t dayTime3;
        uint32_t nightTime;
    };

    struct ticker_info {
        std::string cname;
        std::string ticker_name;
        double multiplier;
        double tick_size;
    };

    struct Q_TickerMetaComposite {
        unsigned int ii;
        char exchange[8];
        char tickerName[8];
        double multiplier;
        double priceTick;
        double rank;

        double OpenRatioByMoney;
        double OpenRatioByVolume;
        double CloseRatioByMoney;
        double CloseRatioByVolume;
        double CloseTodayRatioByMoney;
        double CloseTodayRatioByVolume;

        double LongMarginRatioByMoney;
        double ShortMarginRatioByMoney;

        double upperLimitPrice;
        double lowerLimitPrice;
        double upperLimitPriceNight;
        double lowerLimitPriceNight;

        void out(std::ostream& stream = std::cout) {
            stream << ii << ","
                   << exchange << ","
                   << tickerName << ","
                   << multiplier << ","
                   << priceTick << ","
                   << rank << ","
                   << OpenRatioByMoney << ","
                   << OpenRatioByVolume << ","
                   << CloseRatioByMoney << ","
                   << CloseRatioByVolume << ","
                   << CloseTodayRatioByMoney << ","
                   << CloseTodayRatioByVolume << ","
                   << LongMarginRatioByMoney << ","
                   << ShortMarginRatioByMoney << ","
                   << upperLimitPrice << ","
                   << lowerLimitPrice << ","
                   << upperLimitPriceNight << ","
                   << lowerLimitPriceNight
            << std::endl;
        }
    };

    struct CFFEXMarketDepthUpdate
    {
        int8_t          InstrumentID[31];
        int8_t          UpdateTime[9];
        int32_t         UpdateMillisec;   // 40
        int32_t         Volume;           // 44
        double          LastPrice;        // 48
        double          Turnover;         // 56
        double          OpenInterest;     // 64
        int32_t         BidVolume1;       // 72
        int32_t         AskVolume1;       // 76
        int32_t         BidVolume2;       // 80
        int32_t         AskVolume2;       // 84
        int32_t         BidVolume3;       // 88
        int32_t         AskVolume3;       // 92
        int32_t         BidVolume4;       // 96
        int32_t         AskVolume4;       // 100
        int32_t         BidVolume5;       // 104
        int32_t         AskVolume5;       // 108
        double          BidPrice1;        // 112
        double          AskPrice1;        // 120
        double          BidPrice2;        // 128
        double          AskPrice2;        // 136
        double          BidPrice3;        // 144
        double          AskPrice3;        // 152
        double          BidPrice4;        // 160
        double          AskPrice4;        // 168
        double          BidPrice5;        // 176
        double          AskPrice5;        // 184
        double          HighestPrice;     // 192
        double          LowestPrice;      // 200
        double          OpenPrice;        // 208
        double          ClosePrice;       // 216
        double          UpperLimitPrice;  // 224
        double          LowerLimitPrice;  // 232
        double          SettlementPrice;  // 240
        double          CurrDelta;        // 248
    };

};