#include "actions.h"

#include "actions_player.h"

#include "common.h"
#include "utils/bimap.h"
#include "utils/state_utils.h"
#include "utils/log.h"

#include "components/component_store.h"

namespace {

const Bimap<Actions::Action, state_utils::Event> action_event_map = {
    #define GENERATE_ENUM(action, name) {Actions::Action::action, state_utils::Event::ACTION_##action},
    #include "actions_enum.h"
    #undef GENERATE_ENUM
};

} // namespace

Actions::Actions(std::weak_ptr<ComponentStore> components) :
    Component(components) {
    // Enable everything by default
    setAllEnabled(true);
    return;
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
        if (auto death = getComponent<Death>()) {
            death->setDead();
            return;
        } else {
            LOGW("Action: Missing death component");
        }
    }

    int action_number = static_cast<int>(action);
    if (action_number < static_cast<int>(Action::NUM_ACTIONS) && !isActionEnabled(action)) {
        try {
            LOGD("Action disabled: %s", string_action_map.at(action).c_str());
        } catch (std::out_of_range& e) {
            LOGE("Invalid action %i", action_number);
            throw e;
        }
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

    // Trigger corresponding event
    if (auto state = getComponent<Stateful>()) {
        auto corresponding_event = action_event_map.at(action);
        state->incomingEvent(corresponding_event);
    }
}

void Actions::removeAction(Action action) {
    current_actions_.erase(action);
}

Actions::Action Actions::fromString(const std::string& action) {
    try {
        return string_action_map.at(action);
    } catch (std::out_of_range& e) {
        throw std::invalid_argument(std::string("Unknown action ") + action.c_str());
        return Action::UNKNOWN;
    }
}

std::shared_ptr<Actions> Actions::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File /* file_instance */) {
    std::shared_ptr<Actions> actions;
    if (j.contains("type") && j["type"].get<std::string>() == "player") {
        actions = std::make_shared<ActionsPlayer>(components);
    } else {
        actions = std::make_shared<Actions>(components);
    }

    actions->reloadFromJson(j);
    return actions;
}

void Actions::reloadFromJson(nlohmann::json j, File /* file_instance */) {
    if (!j.contains("enabled_actions")) {
        // Enabled everything by default
        setAllEnabled(true);
        return;
    }

    // If whitelist then disable everything by default
    setAllEnabled(false);

    auto j_actions = j["enabled_actions"];

    // TODO See if possible to use string action map for loop
    if (j_actions.contains("movement_x")) {
        enableAction(Action::MOVE_RIGHT, true);
        enableAction(Action::MOVE_LEFT, true);
    }

    if (j_actions.contains(string_action_map.at(Action::JUMP))) {
        enableAction(Action::JUMP, true);
    }

    if (j_actions.contains(string_action_map.at(Action::WALL_JUMP))) {
        enableAction(Action::WALL_JUMP, true);
    }

    if (j_actions.contains(string_action_map.at(Action::DOUBLE_JUMP))) {
        enableAction(Action::DOUBLE_JUMP, true);
    }

    if (j_actions.contains(string_action_map.at(Action::DASH))) {
        enableAction(Action::DASH, true);
    }

    if (j_actions.contains(string_action_map.at(Action::ATTACK1))) {
        enableAction(Action::ATTACK1, true);
    }

    if (j_actions.contains(string_action_map.at(Action::ATTACK2))) {
        enableAction(Action::ATTACK2, true);
    }

    if (j_actions.contains(string_action_map.at(Action::INTERACT))) {
        enableAction(Action::INTERACT, true);
    }

    if (j_actions.contains(string_action_map.at(Action::AIR_DIVE))) {
        enableAction(Action::AIR_DIVE, true);
    }

    if (j_actions.contains(string_action_map.at(Action::AIR_DIVE_BOUNCE))) {
        enableAction(Action::AIR_DIVE_BOUNCE, true);
    }
}

std::optional<nlohmann::json> Actions::outputToJson() {
    nlohmann::json j;

    // Everything is enabled by default. If any action is not enabled then we store the whitelist.
    if (std::any_of(enabled_actions_.begin(), enabled_actions_.end(), [](bool b) { return !b; })) {
        nlohmann::json j_actions;

        if (isActionEnabled(Action::MOVE_RIGHT) &&
            isActionEnabled(Action::MOVE_LEFT)) {
            j_actions["movement_x"] = true;
        }

        if (isActionEnabled(Action::JUMP)) {
            j_actions[string_action_map.at(Action::JUMP)] = true;
        }

        if (isActionEnabled(Action::WALL_JUMP)) {
            j_actions[string_action_map.at(Action::WALL_JUMP)] = true;
        }

        if (isActionEnabled(Action::DOUBLE_JUMP)) {
            j_actions[string_action_map.at(Action::DOUBLE_JUMP)] = true;
        }

        if (isActionEnabled(Action::DASH)) {
            j_actions[string_action_map.at(Action::DASH)] = true;
        }

        if (isActionEnabled(Action::ATTACK1)) {
            j_actions[string_action_map.at(Action::ATTACK1)] = true;
        }

        if (isActionEnabled(Action::ATTACK2)) {
            j_actions[string_action_map.at(Action::ATTACK2)] = true;
        }

        if (isActionEnabled(Action::INTERACT)) {
            j_actions[string_action_map.at(Action::INTERACT)] = true;
        }

        if (isActionEnabled(Action::AIR_DIVE)) {
            j_actions[string_action_map.at(Action::AIR_DIVE)] = true;
        }

        if (isActionEnabled(Action::AIR_DIVE_BOUNCE)) {
            j_actions[string_action_map.at(Action::AIR_DIVE_BOUNCE)] = true;
        }

        j["enabled_actions"] = j_actions;
    }

    return j;
}

void Actions::enableAction(Action action, bool enable) {
    enabled_actions_[static_cast<int>(action)] = enable;
}

bool Actions::isActionEnabled(Action action) {
    return enabled_actions_[static_cast<int>(action)];
}

void Actions::setAllEnabled(bool enabled) {
    for (auto& it : enabled_actions_) {
        it = enabled;
    }
}
