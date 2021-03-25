#pragma once

#include "simpleini/SimpleIni.h"
#include <common/util.h>

namespace husky {

    struct dynamic_pattern {
        virtual ~dynamic_pattern() {};
        virtual bool is_pattern(const char* v, int len) = 0;
        virtual std::string convert() = 0;
    };
    struct yyyyMMdd_pattern : public dynamic_pattern {
        ~yyyyMMdd_pattern() override {};
        bool is_pattern(const char* v, int len) override {
            return strncmp("yyyyMMdd", v, len) == 0;
        }
        std::string convert() override {
            return husky::local_time_format();
        }
    };
    struct dn_pattern : public dynamic_pattern {
        ~dn_pattern() override {};
        bool is_pattern(const char* v, int len) override {
            return strncmp("dn", v, len) == 0;
        }
        std::string convert() override {
            return husky::dn() == husky::DAY ? "day" : "night";
        }
    };

    class cfg {
    private:
        std::vector<dynamic_pattern*> pattern_collection;
    public:
        ~cfg() {
            for (auto* ptr : pattern_collection) {
                delete ptr;
            }
        }
        cfg(const std::string &path): _file_path(path) {
            _ini.SetUnicode();
            SI_Error rc = _ini.LoadFile(path.c_str());
            if (rc < 0) {
                throw std::invalid_argument("parse " + path + " error");
            };
            pattern_collection.push_back(new yyyyMMdd_pattern());
            pattern_collection.push_back(new dn_pattern());
        }

        void reload() {
            _ini.Reset();
            _ini.LoadFile(_file_path.c_str());
        }

        template<typename N>
        std::vector<N> get_vector(const std::string& key, char delim = ',') {
            std::vector<N> result;
            for (const auto& s: husky::split_by(get_string(key), delim))
                result.push_back(parseStr<N>(s));
            return result;
        }

        const char* get(const std::string& key) const {
            const auto& temp = husky::split_by(key, '.');
            if (temp.size() > 2 || temp.size() == 0) {
                throw std::invalid_argument("cfg key " + key + " error");
            }
            if (temp.size() == 1) {
                return _ini.GetValue("", key.c_str());
            } else {
                return _ini.GetValue(temp[0].c_str(), temp[1].c_str());
            }
        }

        const std::string get_string(const std::string& key, std::string v = "") const {
            const char* val = this->get(key);
            return val == nullptr ? v : val;
        }

        int get_int(const std::string& key, int v = 0) const {
            const char* val = this->get(key);
            return val == nullptr ? v : std::stoi(val);
        }

        unsigned long get_unsigned_long(const std::string& key, int v = 0) const {
            const char* val = this->get(key);
            return val == nullptr ? v : std::stoul(val);
        }

        float get_float(const std::string& key, float v = 0) const {
            const char* val = this->get(key);
            return val == nullptr ? v : std::stof(val);
        }

        bool get_bool(const std::string& key, bool v = false) {
            const char* val = this->get(key);
            return val == nullptr ? v : (strcmp(val, "true") == 0 ? true : false);
        }

        double get_double(const std::string& key, double v = 0) {
            const char* val = this->get(key);
            return val == nullptr ? v : std::stod(val);
        }

        std::string parse_dynamic(std::string str) {
            for (unsigned int i = 0; i < str.size();) {
                char s_type = str[i];
                if (s_type == '#' || s_type == '$') {
                    int replace_begin = i;
                    i += 2;
                    std::vector<char> temp;
                    while (str[i] != '}') {
                        temp.push_back(str[i]);
                        ++i;
                    }
                    temp.push_back('\0');
                    if (s_type == '$') {
                        std::string env_replacement = std::getenv(temp.data());
                        str = str.replace(str.begin() + replace_begin,
                                          str.begin() + replace_begin + temp.size() + 2,
                                          env_replacement);
                        i = replace_begin + env_replacement.length();
                    } else { // '#'
                        for (dynamic_pattern *pattern: pattern_collection) {
                            if (pattern->is_pattern(temp.data(), temp.size())) {
                                std::string replacement = pattern->convert();
                                str = str.replace(str.begin() + replace_begin,
                                                  str.begin() + replace_begin + temp.size() + 2,
                                                  replacement);
                                i = replace_begin + replacement.length();
                                break;
                            }
                        }
                    }
                    temp.clear();
                } else {
                    ++i;
                }
            }
            return str;
        }
    private:
        CSimpleIniA _ini;
        std::string _file_path;
    };
}