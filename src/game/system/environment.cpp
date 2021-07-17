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
    triggered_conditions_.insert(name);

    bool invert = removeNegationChar(name);

    if (!invert) {
        flags_.insert(name);
    } else {
        flags_.erase(name);
    }
}

void Environment::loadEnvFromJson(nlohmann::json j) {
    clearEnv();

    for (auto it : j["flags"]) {
        setFlag(it.get<std::string>());
    }
}

nlohmann::json Environment::outputEnvToJson() {
    nlohmann::json j;

    j["flags"] = flags_;

    return j;
}

void Environment::addConditionalEvent(std::string condition, std::shared_ptr<event_triggers::IEventTrigger> event) {
    conditional_events_.insert({condition, event});
}

void Environment::triggerConditionalEvents() {
    if (!triggered_conditions_.empty()) {
        for (auto cond : triggered_conditions_) {
            while (conditional_events_.count(cond) > 0) {
                auto it = conditional_events_.extract(cond);
                it.mapped()->onEvent();
            }
        }

        triggered_conditions_.clear();
    }
}

void Environment::clearConditionalEvents() {
    triggered_conditions_.clear();
    conditional_events_.clear();
}

void Environment::clearEnv() {
    flags_.clear();
}
