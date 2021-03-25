#ifndef _META_DEFINE_H
#define _META_DEFINE_H

const static char S_META[]                  = "meta";
//const static char S_META_TOP3[]             = "meta_top3";
//const static char S_META_TOP4[]             = "meta_top4";
const static char S_SNAP[]                  = "snap";
const static char S_DI[]                    = "di";
const static char S_DI_REVERSE[]            = "di_reverse";
const static char S_TI[]                    = "ti";
const static char S_TI_REVERSE[]            = "ti_reverse";
const static char S_OPEN_PRICE_SNAPSHOT[]   = "open_price_snapshot";
const static char S_CLOSE_PRICE_SNAPSHOT[]  = "close_price_snapshot";
const static char S_HIGH_PRICE_SNAPSHOT[]   = "high_price_snapshot";
const static char S_LOW_PRICE_SNAPSHOT[]    = "low_price_snapshot";
const static char S_VOLUME_SNAPSHOT[]       = "volume_snapshot";
const static char S_AMOUNT_SNAPSHOT[]       = "amount_snapshot";
const static char S_LAST_OPENINT_SNAPSHOT[] = "last_openint_snapshot";
const static char S_BID_SNAPSHOT[]          = "bid_snapshot";
const static char S_ASK_SNAPSHOT[]          = "ask_snapshot";
const static char S_BIDV_SNAPSHOT[]         = "bidv_snapshot";
const static char S_ASKV_SNAPSHOT[]         = "askv_snapshot";

const static char S_OPEN_PRICE_DAILY[]      = "open_price_daily";
const static char S_CLOSE_PRICE_DAILY[]     = "close_price_daily";
const static char S_HIGH_PRICE_DAILY[]      = "high_price_daily";
const static char S_LOW_PRICE_DAILY[]       = "low_price_daily";
const static char S_VOLUME_DAILY[]          = "volume_daily";
const static char S_AMOUNT_DAILY[]          = "amount_daily";
const static char S_OPENINT_DAILY[]         = "openint_daily";

//const static char S_OPEN_PRICE_DAILY_TOP3[]      = "open_price_daily_top3";
//const static char S_CLOSE_PRICE_DAILY_TOP3[]     = "close_price_daily_top3";
//const static char S_HIGH_PRICE_DAILY_TOP3[]      = "high_price_daily_top3";
//const static char S_LOW_PRICE_DAILY_TOP3[]       = "low_price_daily_top3";
//const static char S_VOLUME_DAILY_TOP3[]          = "volume_daily_top3";
//const static char S_AMOUNT_DAILY_TOP3[]          = "amount_daily_top3";
//const static char S_OPENINT_DAILY_TOP3[]         = "openint_daily_top3";


const static char S_L1_TICK[]               = "tick_l1";
const static char S_L2_TICK[]               = "tick_l2";

const static char S_TRADE_TIME[]            = "trade_time";

const static int nTicker   = 300;
//const static int nTicker   = 150;
const static int nMaxTicks = 54000;
const static int kNTi = 615;

const static char S_DEPS[] = "deps";
const static char S_TRIGGERS[] = "triggers";

const static uint32_t kDayOpenSnap = 90000;
const static uint32_t kDayClose1Snap = 101500;
const static uint32_t kDayReopen1Snap = 103000;
const static uint32_t kDayClose2Snap = 113000;
const static uint32_t kDayFinancialReopen2Snap = 130000;
const static uint32_t kDayCommodityReopen2Snap = 133000;
const static uint32_t kDayClose3Snap = 150000;
const static uint32_t kDayClose4Snap = 151500;
const static uint32_t kNightOpenSnap = 210000;
const static uint32_t kNightCloseSnap = 263000;
#endif