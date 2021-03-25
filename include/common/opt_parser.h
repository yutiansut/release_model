#pragma once

#include <map>
#include <string>
#include <vector>
#include <sstream>
/*
 * Opt Parser
 *
 * Usage: "./main.o --key1=val1 --key2=val2"
 *
 */
namespace husky {
    struct Opt {
        std::string description;
        std::string value;

        bool empty() { return value.empty(); }               // check is opt is empty
        std::string get() { return value; }
        int getAsInt() { return stoi(value); }
        unsigned int getAsUnsigned() { return stoul(value); }
        double getAsDouble() { return stod(value); }
        float getAsFloat() { return stod(value); }
        bool getAsBool() { return value == "true"; }
        std::vector<std::string> getAsVector(char delim = ',') {
            std::stringstream ss(value);
            std::vector<std::string> ret;
            while(ss.good()) {
                std::string substr;
                getline(ss, substr, delim);
                if (!substr.empty()) {
                    ret.push_back(substr);
                }
            }
            return ret;
        };
    };

    class OptParser {
    public:
        void addOption(const std::string& key, const std::string& description);
        void parse(int argc, char** argv);
        Opt getOpt(const std::string& key);
        bool hasOpt(const std::string& key);
        void getHelp();
    private:
        std::map<std::string, Opt> optContainer;
    };
}