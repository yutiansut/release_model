#pragma once

#include <tools.h>

namespace husky {
    namespace contract {

        struct Q_Contract {
            char tickerName[8];
            char contractName[8];
            int rank;
        };

        HUSKY_INLINE std::pair<std::string, std::string> extractFromContractName(const std::string &contract_name) {
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
            for (const char &c :contract_number_str) {
                if (!isdigit(c)) {
                    return {"", ""};
                }
            }
            return {ticker_name, contract_number_str};
        }

        HUSKY_INLINE std::map<std::string, std::list<Q_Contract>> loadContract(const std::string &filePath, int topLimit) {
            std::map<std::string, std::list<Q_Contract>> map;
            const std::vector<std::string> &lines = husky::f2str_vec(filePath);
            for (const std::string &line: lines) {
                const std::vector<std::string> &contracts = husky::split_by(line, ',');
                int top = 0;
                for (const std::string &contract: contracts) {
                    if (!contract.empty()) {
                        top++;
                        if (top > topLimit) break;
                        const std::pair<std::string, std::string> &pair = extractFromContractName(contract);
                        if (map.find(pair.first) == map.end()) {
                            map[pair.first] = {};
                        }
                        Q_Contract qContract{};
                        strcpy(qContract.tickerName, pair.first.c_str());
                        strcpy(qContract.contractName, contract.c_str());
                        qContract.rank = top;
                        map.at(pair.first).push_back(qContract);
                    }
                }
            }
            return map;
        }

        HUSKY_INLINE std::map<std::string, std::list<Q_Contract>> loadLatestContract(const std::string &contractPath, int topLimit) {
            std::string newest_contract = husky::list_dir(contractPath, {"contract\\.\\d+$"}).back();
            const auto &contract_info_path = husky::slash_path(contractPath) + newest_contract;
            return loadContract(contract_info_path, topLimit);
        }

        HUSKY_INLINE std::map<std::string, std::string> contracts_mapper(const std::string& contractPath) {
            std::string newest_contract = husky::list_dir(contractPath, {"contract\\.\\d+$"}).back();
            const auto &contract_info_path = husky::slash_path(contractPath) + newest_contract;
            const auto &contract_lines = husky::f2str_vec(contract_info_path.c_str());
            const auto &contract_mapper = husky::mapper::contract_mapper(contract_lines);
            return contract_mapper;
        }

    }
}