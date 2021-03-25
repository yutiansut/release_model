#pragma once

#include <exception>
#include <string>

namespace husky {
    class IIOutOfBoundaryException: public std::exception {
    private:
        std::string boundaryContainerName;
    public:
        explicit IIOutOfBoundaryException(const std::string& boundaryContainerName): boundaryContainerName(std::move(boundaryContainerName)) {

        };

        virtual const char* what() const throw() {
            return boundaryContainerName.c_str();
        }
    };


}