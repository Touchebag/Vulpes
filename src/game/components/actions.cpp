#include "actions.h"

void Actions::update() {
    for (auto &it : current_actions_) {
        it.second = false;
    }
}

bool Actions::getActionState(Action action, bool first_frame) {
    auto it = current_actions_.find(action);
    return (it != current_actions_.end() && (!first_frame || it->second));
}

void Actions::addAction(Action action) {
    current_actions_.insert_or_assign(action, true);
}

void Actions::removeAction(Action action) {
    current_actions_.erase(action);
}

void Actions::loadFromJson(nlohmann::json j) {
}

std::optional<nlohmann::json> Actions::outputToJson() {
    nlohmann::json j;
    return j;
}
