#pragma once

#include <exception>
#include <string>

namespace husky {
    class UnimplementedException: public std::exception {
    private:
        std::string _message;
    public:
        explicit UnimplementedException(const std::string& message): _message(std::move(message)) {

        };

        virtual const char* what() const throw() {
            return _message.c_str();
        }
    };


}