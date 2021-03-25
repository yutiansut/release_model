#pragma once

#include <exception>
#include <string>

namespace husky {
    class FdNotExistException: public std::exception {
    private:
        std::string msg;
    public:
        explicit FdNotExistException(const std::string& str) {
            msg = "file or fd: " + str + " not not exist";
        };

        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };


}