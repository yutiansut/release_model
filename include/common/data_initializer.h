#pragma once

#include <common/husky_matrix.h>
#include <cstdint>

namespace husky { namespace data_initializer {

inline static void tiInitialize(husky::basic_matrix<uint32_t>& tiMatrix) {
    unsigned int currentTi = 0;
    for (unsigned hour = 9; hour <= 15; ++hour) {
        if (hour == 12) continue;
        if (hour == 11) {
            for (unsigned minute = 0; minute <= 30; ++minute) {
                tiMatrix(currentTi, 0) = hour * 10000 + minute * 100;
                currentTi++;
            }
        } else if (hour == 13) {
            for (unsigned minute = 1; minute <= 59; ++minute) {
                tiMatrix(currentTi, 0) = hour * 10000 + minute * 100;
                currentTi++;
            }
        } else if (hour == 15) {
            for (unsigned minute = 0; minute <= 15; ++minute) {
                tiMatrix(currentTi, 0) = hour * 10000 + minute * 100;
                currentTi++;
            }
        } else {
            for (unsigned minute = 0; minute <= 59; ++minute) {
                if (hour == 9 && minute == 0) {
                    continue;
                }
                tiMatrix(currentTi, 0) = hour * 10000 + minute * 100;
                currentTi++;
            }
        }
    }
    for (unsigned hour = 21; hour <= 26; ++hour) {
        if (hour == 26) {
            for (unsigned minute = 0; minute <= 30; ++minute) {
                tiMatrix(currentTi, 0) = hour * 10000 + minute * 100;
                currentTi++;
            }
        } else {
            for (unsigned minute = 0; minute <= 59; ++minute) {
                if (hour == 21 && minute == 0) {
                    continue;
                }
                tiMatrix(currentTi, 0) = hour * 10000 + minute * 100;
                currentTi++;
            }
        }
    }
}

}}