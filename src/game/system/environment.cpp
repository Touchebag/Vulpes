#include "environment.h"

#include "system/system.h"

#include "utils/log.h"

namespace {

bool removeNegationChar(std::string& name) {
    bool invert = false;

    // If condition starts with '!', invert the result
    if (name.at(0) == '!') {
        invert = true;
        name = name.erase(0, 1);
    }

    // Return true if should be inverted
    return invert;
}

} // namespace

bool Environment::getFlag(std::string name) {
    bool invert = removeNegationChar(name);

    bool flag = flags_.count(name);
    return invert ? !flag : flag;
}

void Environment::setFlag(std::string name) {
    // Flag with negation sign first
    System::IWorldModify::addConditionalEntities(name);

    bool invert = removeNegationChar(name);

    if (!invert) {
        flags_.insert(name);
    } else {
        flags_.erase(name);
    }
}
