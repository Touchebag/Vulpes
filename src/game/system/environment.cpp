#include "environment.h"

#include "utils/log.h"

bool Environment::getFlag(std::string name) {
    bool invert = false;

    // If condition starts with '!', invert the result
    if (name.at(0) == '!') {
        invert = true;
        name = name.erase(0, 1);
    }

    bool flag = flags_.count(name);
    return invert ? !flag : flag;
}

void Environment::setFlag(std::string name) {
    flags_.insert(name);
}
