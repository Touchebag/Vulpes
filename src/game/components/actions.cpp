#include "actions.h"

#include "utils/log.h"
#include "utils/bimap.h"

namespace {

const Bimap<std::string, Actions::Action> string_action_map = {
    {"unknown", Actions::Action::UNKNOWN},
    {"move_left", Actions::Action::MOVE_LEFT},
    {"move_right", Actions::Action::MOVE_RIGHT},
    {"jump", Actions::Action::JUMP},
    {"dash", Actions::Action::DASH},
    {"attack", Actions::Action::ATTACK},

    {"ai_event_1", Actions::Action::AI_EVENT_1},
    {"ai_event_2", Actions::Action::AI_EVENT_2},

    {"die", Actions::Action::DIE},
};

}

Actions::Actions(std::weak_ptr<Death> death) :
    death_(death) {
}

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
    if (action == Action::DIE) {
        if (auto death = death_.lock()) {
            death->setDead();
        }
    }

    if (!enabled_actions_[static_cast<int>(action)]) {
        LOGD("Action disabled: %s", string_action_map.at(action).c_str());
        return;
    }

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

Actions::Action Actions::fromString(const std::string& action) {
    try {
        return string_action_map.at(action);
    } catch (std::out_of_range& e) {
        throw std::invalid_argument(std::string("Unknown action ") + action.c_str());
        return Action::UNKNOWN;
    }
}

void Actions::reloadFromJson(nlohmann::json j) {
    if (j.contains("movement_x")) {
        enabled_actions_[static_cast<int>(Action::MOVE_RIGHT)] = true;
        enabled_actions_[static_cast<int>(Action::MOVE_LEFT)] = true;
    }

    if (j.contains(string_action_map.at(Action::JUMP))) {
        enabled_actions_[static_cast<int>(Action::JUMP)] = true;
    }

    if (j.contains(string_action_map.at(Action::DASH))) {
        enabled_actions_[static_cast<int>(Action::DASH)] = true;
    }

    if (j.contains(string_action_map.at(Action::ATTACK))) {
        enabled_actions_[static_cast<int>(Action::ATTACK)] = true;
    }
}

std::optional<nlohmann::json> Actions::outputToJson() {
    nlohmann::json j;

    if (enabled_actions_[static_cast<int>(Action::MOVE_RIGHT)] &&
        enabled_actions_[static_cast<int>(Action::MOVE_LEFT)]) {
        j["movement_x"] = true;
    }

    if (enabled_actions_[static_cast<int>(Action::JUMP)]) {
        j[string_action_map.at(Action::JUMP)] = true;
    }

    if (enabled_actions_[static_cast<int>(Action::DASH)]) {
        j[string_action_map.at(Action::DASH)] = true;
    }

    if (enabled_actions_[static_cast<int>(Action::ATTACK)]) {
        j[string_action_map.at(Action::ATTACK)] = true;
    }

    return j;
}
