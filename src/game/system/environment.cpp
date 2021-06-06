#include "environment.h"

#include "utils/log.h"

bool Environment::getFlag(std::string name) {
    return flags_.count(name);
}

void Environment::setFlag(std::string name) {
    flags_.insert(name);
}
