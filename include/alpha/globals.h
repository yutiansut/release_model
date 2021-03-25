#pragma once

#include <common/cfg.h>
#include <compact.h>

/**
 * Global Singleton
 * Support save and load global states
 */
namespace husky {

typedef uint8_t AlphaMode;
const AlphaMode CTA_MODE = 1 << 0;    // 00000001
const AlphaMode STOCK_MODE = 1 << 1;  // 00000010

#define MD_CONFIG husky::globals::getMdConfig()
#define ALPHA_CONFIG husky::globals::getAlphaConfig()
#define OB husky::globals::getOrderBook()
#define ALPHA_ID husky::globals::getAlphaId()
#define ALPHA_ID_LENGTH husky::globals::getAlphaLen()
#define Q_DI husky::globals::getDi()
#define Q_TI husky::globals::getTi()
#define Q_SI husky::globals::getSi()
#define Q_REPLAY husky::globals::IsReplay()
#define Q_TRAIN_SNAPSHOT husky::globals::IsTrainSnapshot()

class globals {
   public:
    globals(const globals &) = delete;

    globals &operator=(const globals &) = delete;

    static globals &get_instance();

    static void setMdConfig(cfg *config);

    static void setAlphaConfig(cfg *config);

    static void setOrderBook(void *ob);

    static void setAlphaId(const std::string &id);

    static void setQdi(uint32_t di);

    static void setQti(uint32_t ti);

    static void setQsi(uint32_t si);

    static void setReplay(bool b);

    static void setTrainSnapshot(bool b);

    static cfg *getMdConfig();

    static cfg *getAlphaConfig();

    static void *getOrderBook();

    static const char *getAlphaId();

    HUSKY_INLINE int getAlphaLen() { return 5; }

    static uint32_t getDi();

    static uint32_t getTi();

    static uint32_t getSi();

    static bool IsReplay();

    static bool IsTrainSnapshot();

   private:
    globals(){};

    cfg *md_config;
    cfg *alpha_config;
    void *order_book;
    bool debug_mode;
    char alphaId[5];
    uint32_t q_di, q_ti, q_si;
    AlphaMode alpha_mode;
    bool isReplay;
    bool isTrainSnapshot;
};

}  // namespace husky
