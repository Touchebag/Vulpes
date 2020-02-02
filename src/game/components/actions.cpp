#include "actions.h"

#include "utils/log.h"

// TODO Cleanup and reuse internal functions

void Actions::update() {
    for (auto it = current_actions_.begin(); it != current_actions_.end();) {
        switch(it->second) {
            case ActionState::ACTIVE:
            case ActionState::FIRST_FRAME:
                it->second = ActionState::NOT_ACTIVE;
                it++;
                break;
            case ActionState::NOT_ACTIVE:
                it = current_actions_.erase(it);
                break;
        }
    }
}

bool Actions::getActionState(Action action, bool first_frame) {
    auto it = current_actions_.find(action);
    return (it != current_actions_.end() &&
            it->second != ActionState::NOT_ACTIVE &&
            (!first_frame || it->second == ActionState::FIRST_FRAME));
}

void Actions::addAction(Action action) {
    auto it = current_actions_.find(action);

    // If action is already in map, update to still be active
    // else add new first frame action
    if (it != current_actions_.end()) {
            it->second = ActionState::ACTIVE;
    } else {
        current_actions_.insert_or_assign(action, ActionState::FIRST_FRAME);
    }
}

void Actions::removeAction(Action action) {
    current_actions_.erase(action);
}

void Actions::updateActions() {
    for (auto it : current_actions_) {
        // Only refresh non-active actions to avoid messing up first frames
        if (it.second == ActionState::NOT_ACTIVE) {
            addAction(it.first);;
        }
    }
}

void Actions::loadFromJson(nlohmann::json j) {
}

std::optional<nlohmann::json> Actions::outputToJson() {
    nlohmann::json j;
    return j;
}
