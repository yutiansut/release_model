#pragma once
#include <string>
namespace husky {

class Identity {
public:
    virtual const std::string name() = 0;
};

}