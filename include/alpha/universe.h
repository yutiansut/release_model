#ifndef _HUSKY_UNIVERSE_H_
#define _HUSKY_UNIVERSE_H_

#include <alpha/globals.h>
#include <common/define.h>
#include <common/husky_matrix.h>

namespace husky {

/**
 * MD Data Universe (Singleton)
 * Provide:
 * 1. data provider
 * 2. support data registry and custom data registry
 * 3. useful macro to access data
 * 4. today database
 */
class Universe {
public:
    ~Universe();
    Universe(const Universe&) = delete;
    Universe& operator= (const Universe&) = delete;

    static void initHft();
    static void initCta();
    static bool hasMatrix(std::string&& name);
    static void ctaLoadDynamicTick(const std::string& yyyyMMdd, DN dn);
    static void registerCustomData(const std::string& name, void* p);
    static void* getCustomData(const std::string& name);

    // get some meta data
    HUSKY_INLINE basic_matrix<md::Q_TickerMetaComposite>& getTickerMeta() { return tickerMetaMatrix; }
    HUSKY_INLINE basic_matrix<uint32_t>&                  getDiMatrix()   { return diMatrix; }
    HUSKY_INLINE basic_matrix<uint32_t>&                  getTiMatrix()   { return tiMatrix; }
    HUSKY_INLINE basic_matrix<md::TradeTime>&             getTradeTimeMatrix() { return tradeTimeMatrix; }

    // get snapshot data cube
    HUSKY_INLINE basic_cube<double> &                     getOSCube()       { return osCube;       }
    HUSKY_INLINE basic_cube<double> &                     getCSCube()       { return csCube;       }
    HUSKY_INLINE basic_cube<double> &                     getHSCube()       { return hsCube;       }
    HUSKY_INLINE basic_cube<double> &                     getLSCube()       { return lsCube;       }
    HUSKY_INLINE basic_cube<int32_t>&                     getVSCube()       { return vsCube;       }
    HUSKY_INLINE basic_cube<double> &                     getASCube()       { return asCube;       }
    HUSKY_INLINE basic_cube<double> &                     getISCube()       { return isCube;       }
    HUSKY_INLINE basic_cube<double> &                     getBidSnapCube()  { return bidSnapCube;  }
    HUSKY_INLINE basic_cube<double> &                     getAskSnapCube()  { return askSnapCube;  }
    HUSKY_INLINE basic_cube<int32_t>&                     getBidvSnapCube() { return bidvSnapCube; }
    HUSKY_INLINE basic_cube<int32_t>&                     getAskvSnapCube() { return askvSnapCube; }

    // get daily data cube
    HUSKY_INLINE basic_matrix<int32_t>&                   getVolumeMatrix()  { return volumeMatrix;  }
    HUSKY_INLINE basic_matrix<double> &                   getOpenMatrix()    { return openMatrix;    }
    HUSKY_INLINE basic_matrix<double> &                   getCloseMatrix()   { return closeMatrix;   }
    HUSKY_INLINE basic_matrix<double> &                   getHighMatrix()    { return highMatrix;    }
    HUSKY_INLINE basic_matrix<double> &                   getLowMatrix()     { return lowMatrix;     }
    HUSKY_INLINE basic_matrix<double> &                   getAmountMatrix()  { return amountMatrix;  }
    HUSKY_INLINE basic_matrix<double> &                   getOpenintMatrix() { return openintMatrix; }

    // get tick data
    HUSKY_INLINE basic_matrix<md::l2>&                    getL1TickMatrix() { return l1TickMatrix; }
    HUSKY_INLINE basic_matrix<md::l2>&                    getL2TickMatrix() { return l2TickMatrix; }

    template<typename T>
    HUSKY_INLINE basic_matrix<T>& getMatrix(const std::string& name) {
        Universe& uni = getInstance();
        if (uni.dataMapper.find(name) == uni.dataMapper.end())
            std::runtime_error("cannot load from universe: " + name);
        void* p = uni.dataMapper.at(name);
        return *((basic_matrix<T>*) p);
    }

    template<typename T>
    HUSKY_INLINE void getMatrix(const std::string& name, basic_matrix<T> *p) {
        Universe& uni = getInstance();
        if (uni.dataMapper.find(name) == uni.dataMapper.end())
            std::runtime_error("cannot load from universe: " + name);
        basic_matrix<T>* pp = (basic_matrix<T>*) uni.dataMapper.at(name);
        if (pp != nullptr) {
            *p = *pp;
        }
    }

    template<typename T>
    HUSKY_INLINE basic_cube<T>& getCube(const std::string& name) {
        Universe& uni = getInstance();
        if (uni.dataMapper.find(name) == uni.dataMapper.end())
            std::runtime_error("cannot load from universe: " + name);
        void* p = uni.dataMapper.at(name);
        return *((basic_cube<T>*) p);
    }

    template<typename T>
    HUSKY_INLINE void getCube(const std::string& name, basic_matrix<T> *p) {
        *p = getCube<T>(name);
    }

    HUSKY_INLINE std::map<uint32_t, uint32_t>& getTiReverseMap() {
        return getInstance().tiReverseMap;
    }

private:
    Universe() {}
    static Universe& getInstance();
    static basic_matrix<uint32_t>                  diMatrix;
    static basic_matrix<uint32_t>                  tiMatrix;
    static basic_matrix<md::Q_TickerMetaComposite> tickerMetaMatrix;
    static basic_matrix<md::l2>                    l1TickMatrix;
    static basic_matrix<md::l2>                    l2TickMatrix;
    static basic_matrix<md::TradeTime>             tradeTimeMatrix;

    static basic_cube<double>                      osCube;
    static basic_cube<double>                      csCube;
    static basic_cube<double>                      hsCube;
    static basic_cube<double>                      lsCube;
    static basic_cube<int32_t>                     vsCube;
    static basic_cube<double>                      asCube;
    static basic_cube<double>                      isCube;
    static basic_cube<double>                      bidSnapCube;
    static basic_cube<double>                      askSnapCube;
    static basic_cube<int32_t>                     bidvSnapCube;
    static basic_cube<int32_t>                     askvSnapCube;
    static basic_matrix<int32_t>                   volumeMatrix;
    static basic_matrix<double>                    openMatrix;
    static basic_matrix<double>                    closeMatrix;
    static basic_matrix<double>                    highMatrix;
    static basic_matrix<double>                    lowMatrix;
    static basic_matrix<double>                    amountMatrix;
    static basic_matrix<double>                    openintMatrix;

    std::map<uint32_t, uint32_t>                   tiReverseMap;
    std::map<std::string, void*>                   dataMapper;
    std::map<std::string, void*>                   customDataMapper;
};

#define HUSKY_DI         (husky::Universe::getDiMatrix())
#define HUSKY_TI         (husky::Universe::getTiMatrix())
#define HUSKY_META       (husky::Universe::getTickerMeta())
#define HUSKY_TRADE_TIME (husky::Universe::getTradeTimeMatrix())

#define HUSKY_OS         (husky::Universe::getOSCube())
#define HUSKY_CS         (husky::Universe::getCSCube())
#define HUSKY_HS         (husky::Universe::getHSCube())
#define HUSKY_LS         (husky::Universe::getLSCube())
#define HUSKY_VS         (husky::Universe::getVSCube())
#define HUSKY_AS         (husky::Universe::getASCube())
#define HUSKY_IS         (husky::Universe::getISCube())
#define HUSKY_BID_SNAP   (husky::Universe::getBidSnapCube())
#define HUSKY_ASK_SNAP   (husky::Universe::getAskSnapCube())
#define HUSKY_BIDV_SNAP  (husky::Universe::getBidvSnapCube())
#define HUSKY_ASKV_SNAP  (husky::Universe::getAskvSnapCube())

#define HUSKY_VD         (husky::Universe::getVolumeMatrix())
#define HUSKY_OD         (husky::Universe::getOpenMatrix())
#define HUSKY_CD         (husky::Universe::getCloseMatrix())
#define HUSKY_HD         (husky::Universe::getHighMatrix())
#define HUSKY_LD         (husky::Universe::getLowMatrix())
#define HUSKY_AD         (husky::Universe::getAmountMatrix())
#define HUSKY_ID         (husky::Universe::getOpenintMatrix())

#define HUSKY_L1_TICK    (husky::Universe::getL1TickMatrix())
#define HUSKY_L2_TICK    (husky::Universe::getL2TickMatrix())
}

#endif
