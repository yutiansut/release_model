#pragma once

#include <compact.h>
//#include <pcrecpp.h>

#include "file_utils.h"

namespace husky {

    HUSKY_INLINE std::string slash_path(const std::string& file_path) {
        std::string ret = file_path;
        if (ret[file_path.size() - 1] != '/')
            ret += '/';
        return ret;
    }

    typedef short DN;
    const short DAY = 0;
    const short NIGHT = 1;
    const short UNKNOWN = 2;

    // 时间相关方法
    HUSKY_INLINE void set_rt(uint64_t& ts) {
        timespec rt{};
        clock_gettime(CLOCK_REALTIME, &rt);
        ts = (uint64_t) rt.tv_sec * 1000000000 + rt.tv_nsec;
    }

    HUSKY_INLINE tm &local_tm() {
        time_t raw_time;
        struct tm *time_info;
        time(&raw_time);
        time_info = localtime(&raw_time);
        return *time_info;
    }

    HUSKY_INLINE std::string local_time_format() {
        struct tm now = local_tm();
        char buffer[9];
        strftime(buffer, 9, "%Y%m%d", &now);
        return std::string(buffer);
    }

    HUSKY_INLINE std::string tm2str(struct tm* tm) {
        char buffer[9];
        strftime(buffer, 9, "%Y%m%d", tm);
        return std::string(buffer);
    }

    HUSKY_INLINE std::string tomorrow_time_format() {
        struct tm now = local_tm();
        now.tm_mday++;
        char buffer[9];
        strftime(buffer, 9, "%Y%m%d", &now);
        return std::string(buffer);
    }

    HUSKY_INLINE std::string working_day_tomorrow_time_format() {
        struct tm now = local_tm();
        if (now.tm_wday == 5) { // Friday
            now.tm_mday += 3;
        } else if (now.tm_wday == 6) {
            now.tm_mday += 2;
        } else {
            now.tm_mday++;
        }
        const time_t temp = mktime(&now);
        return husky::tm2str(localtime(&temp));
    }

    HUSKY_INLINE std::string working_day_yesterday_time_format() {
        struct tm now = local_tm();
        if (now.tm_wday == 1) {
            now.tm_wday -= 3;
        } else {
            now.tm_mday--;
        }
        const time_t temp = mktime(&now);
        return husky::tm2str(localtime(&temp));
    }

    HUSKY_INLINE std::string local_time_format(const std::string &format) {
        struct tm now = local_tm();
        char buffer[20];
        strftime(buffer, 20, format.c_str(), &now);
        return std::string(buffer);
    }

    HUSKY_INLINE time_t current_time() {
        time_t raw_time;
        return time(&raw_time);
    }

    HUSKY_INLINE std::string time64_format(uint64_t i) {
        time_t t = i / 1000000000;
        tm* local_tm = std::localtime(&t);
        auto sup_zero = [](int x, uint len) {
            std::string result = std::to_string(x);
            while (result.size() < len) {
                result = "0" + result;
            }
            return result;
        };
        std::string ret;
        ret += sup_zero(local_tm->tm_hour, 2);
        ret += ":" + sup_zero(local_tm->tm_min, 2);
        ret += ":" + sup_zero(local_tm->tm_sec, 2);
        ret += "." + sup_zero(i/1000000-t*1000, 3);
        return ret;
    }

    HUSKY_INLINE unsigned int tsNano2humanSec(uint64_t ts) {
        const time_t snap_sec = ts / 1000000000;
        tm *currentTm = localtime( &snap_sec );
        return currentTm->tm_hour * 10000 + currentTm->tm_min * 100 + currentTm->tm_sec;
    }

    HUSKY_INLINE int get_time(int unix_time, int millisecond) {
        time_t ut = (time_t)unix_time;
        struct tm tm;
        tm = *localtime(&ut);
        return tm.tm_hour * 10000000+ tm.tm_min * 100000 + tm.tm_sec * 1000 + millisecond;
    }

    HUSKY_INLINE uint32_t s2i(const char *p, int len) {
        uint32_t x = 0;
        for (int i = 0; i < len; ++i) {
            x = (x * 10) + (*p - '0');
            ++p;
        }
        return x;
    }

    HUSKY_INLINE DN dn() {
        const tm &tm = local_tm();
        if (tm.tm_hour >= 6 && tm.tm_hour <= 18) {
            return DAY;
        } else {
            return NIGHT;
        }
    }

    inline std::string dn_s() {
        return dn() == DAY ? "D" : "N";
    }

    inline std::string get_dn_string() {
        return dn() == DAY ? "day" : "night";
    }

    HUSKY_INLINE bool is_dn_day(const std::string& x) {
        return x == "D";
    }

    HUSKY_INLINE std::string local_time_dn() {
        return local_time_format() + "." + (dn()==DAY?"D":"N");
    }

    HUSKY_INLINE time_t today_midnight() {
        tm tm = local_tm();
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;
        return mktime(&tm);
    }

    HUSKY_INLINE int64_t timespec_diff_ns(struct timespec a, struct timespec b) {
        return (a.tv_sec - b.tv_sec) * (int64_t) 1000000000 + (a.tv_nsec - b.tv_nsec);
    }
    // ++++++++++++++++++++++++++++++++++++

    // string相关方法
    template <typename T>
    std::string toStrWithPrecision(const T a_value, const int n = 1) {
        std::ostringstream out;
        out.precision(n);
        out << std::fixed << a_value;
        return out.str();
    }

    HUSKY_INLINE std::vector<std::string> split_by(const std::string& s, char delim) {
        std::stringstream ss(s);
        std::vector<std::string> ret;
        while(ss.good()) {
            std::string substr;
            getline(ss, substr, delim);
            if (!substr.empty()) {
                ret.push_back(substr);
            }
        }
        return ret;
    }

    template<typename Container>
    HUSKY_INLINE void split_by(const std::string &s, char delim, Container& container) {
        std::stringstream ss(s);
        while(ss.good()) {
            std::string substr;
            getline(ss, substr, delim);
            container.insert(container.end(), substr);
        }
    }

    HUSKY_INLINE char* concat(const char *s1, const char *s2) {
        char *result = (char*) malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
        strcpy(result, s1);
        strcat(result, s2);
        return result;
    }

    HUSKY_INLINE std::string string_concat(const char* s1, const char *s2) {
        return std::string(s1).append(s2);
    }

    HUSKY_INLINE std::pair<std::string, std::string> extractContractName(const std::string& contract_name) {
        std::string ticker_name;
        unsigned i;
        for (i = 0; i < contract_name.size(); ++i) {
            if (!isdigit(contract_name[i])) {
                ticker_name += contract_name[i];
            } else {
                break;
            }
        }
        std::string contract_number_str = contract_name.substr(i);
        for (const char& c :contract_number_str) {
            if (!isdigit(c)) {
                return {"", ""};
            }
        }
        return {ticker_name, contract_number_str};
    }

    namespace set {
        HUSKY_INLINE std::set<std::string> split_by(const std::string& s, char delim) {
            std::stringstream ss(s);
            std::set<std::string> ret;
            while(ss.good()) {
                std::string substr;
                getline(ss, substr, delim);
                ret.insert(substr);
            }
            return ret;
        }

        template<typename T>
        HUSKY_INLINE bool contains(const std::set<T>& set, const T& value) {
            return set.find(value) != set.end();
        }
    }


    HUSKY_INLINE std::string to_lower(const std::string& s) {
        std::string data = s;
        transform(data.begin(), data.end(), data.begin(), ::tolower);
        return data;
    }

    HUSKY_INLINE std::string to_upper(const std::string& s) {
        std::string data = s;
        transform(data.begin(), data.end(), data.begin(), ::toupper);
        return data;
    }

    HUSKY_INLINE std::string extract_ticker_name(const std::string& contract_sym) {
        std::string result;
        for (const char& c: contract_sym) {
            if (!isdigit(c)) {
                result += c;
            } else {
                break;
            }
        }
        return result;
    }

    // 二进制相关方法
    HUSKY_INLINE const char *byte_to_binary(int x) {
        char *b = new char[9];
        b[0] = '\0';
        int z;
        for (z = 128; z > 0; z >>= 1) {
            strcat(b, ((x & z) == z) ? "1" : "0");
        }
        return b;
    }

    template <typename T>
    HUSKY_INLINE size_t indexof(const std::vector<T> &vec, T target) {
        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] == target) {
                return i;
            }
        }
        return -1;
    }

    extern std::vector<std::string> list_dir(const std::string &path, std::vector<std::string> regex_s);

    namespace map {
        template<typename KEY, typename VALUE>
        HUSKY_INLINE void p(const std::unordered_map<KEY,VALUE>& map) {
            for (const auto& entry: map) std::cout << entry.first << "," << entry.second << std::endl;
        }

        template<typename KEY, typename VALUE>
        HUSKY_INLINE void p(const std::map<KEY,VALUE>& map) {
            for (const auto& entry: map) std::cout << entry.first << "," << entry.second << std::endl;
        }

        template<typename MAP, typename KEY>
        HUSKY_INLINE bool key_exist(const MAP& map, KEY k) {
            return map.find(k) != map.end();
        }

        template<typename KEY, typename VALUE>
        HUSKY_INLINE void insert_if_not_exist(KEY k, VALUE v, std::map<KEY, VALUE>& map) {
            if (map.find(k) == map.end()) {
                map[k] = v;
            }
        }
    }

    namespace mapper {
        HUSKY_INLINE std::map<std::string, std::string> contract_mapper(const std::vector<std::string>& contracts_info) {
            std::map<std::string, std::string> ret;
            for (const auto& line: contracts_info) {
                const auto& contracts = husky::split_by(line, ',');
                for (unsigned int i = 0; i < contracts.size(); ++i) {
                    if (contracts[i].empty()) continue;
                    std::string ticker_name = husky::extract_ticker_name(contracts[i]);
                    ret[ticker_name + std::to_string(i + 1)] = contracts[i];
                }
            }
            return ret;
        }
    }

    namespace checker {
        const char LT = '1';
        const char GT = '2';
        const char LTE = '3';
        const char GTE = '4';
        const char EQ = '5';
        HUSKY_INLINE void cmp_check(int i, int j, char flag) {
            if (flag == LT) {
                if (i >= j) {
                    throw std::runtime_error(std::string("cmp checker error!!!"));
                }
            } else if (flag == LTE) {
                if (i > j) {
                    throw std::runtime_error(std::string("cmp checker error!!!"));
                }
            } else if (flag == GT) {
                if (i <= j) {
                    throw std::runtime_error(std::string("cmp checker error!!!"));
                }
            } else if (flag == GTE) {
                if (i < j) {
                    throw std::runtime_error(std::string("cmp checker error!!!"));
                }
            } else if (flag == EQ) {
                if (i != j) {
                    throw std::runtime_error(std::string("cmp checker error!!!"));
                }
            }
        }

        template<typename T>
        HUSKY_INLINE bool eq(const std::vector<T>& values) {
            T first = values[0];
            for (unsigned int i = 1; i < values.size(); ++i) {
                if (values[i] != first) return false;
            }
            return true;
        }

        HUSKY_INLINE void fd_check(long int x) {
            if (x < 0) {
                perror("");
                throw std::runtime_error(std::string("fd checker error!!!"));
            }
        }

        HUSKY_INLINE void non_negative_check(long int x) {
            if (x < 0) {
                perror("");
                throw std::runtime_error(std::string("non_negative checker error!!!"));
            }
        }

        HUSKY_INLINE void sys_check(int x, const char* msg) {
            if (x < 0) {
                perror("");
                throw std::runtime_error(msg);
            }
        }

        HUSKY_INLINE void mmap_check(void *p) {
            if (p == (void*) -1) {
                perror("");
                throw std::runtime_error(std::string("mmap checker error!!!"));
            }
        }

        template<typename N>
        HUSKY_INLINE bool in_range_inclusive(N a, N b, N val) {
            return val >= a && val <= b;
        }

        template<typename N>
        HUSKY_INLINE bool in_range_exclusive(N a, N b, N val) {
            return val > a && val < b;
        }
    }
#define MAP_EXIST_KEY(M, K) (M.find(K) != M.end())
}