#include "actions.h"

#include "common.h"
#include "system/system.h"
#include "utils/bimap.h"
#include "components/state/state_utils.h"
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
        switch(it->second.active) {
            case ActionState::ActiveState::ACTIVE:
                it->second.active = ActionState::ActiveState::NOT_ACTIVE;
                it->second.frame_count++;
                it++;
                break;
            case ActionState::ActiveState::NOT_ACTIVE:
                it = current_actions_.erase(it);
                break;
        }
    }
}

bool Actions::getActionState(Action action, bool first_frame) {
    const int BUFFER_SIZE = 1;

    auto it = current_actions_.find(action);
    return (it != current_actions_.end() &&
            isActionEnabled(action) &&
           (!first_frame || it->second.frame_count <= BUFFER_SIZE)); // This allows for a one frame buffer
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

    if (action >= Action::NUM_ACTIONS) {
        LOGE("Invalid action %i", static_cast<int>(action));
        return;
    }

    auto it = current_actions_.find(action);

    // If action is already in map, update to still be active
    // else add new first frame action
    if (it != current_actions_.end()) {
        it->second.active = ActionState::ActiveState::ACTIVE;
    } else {
        ActionState action_state;
        action_state.active = ActionState::ActiveState::ACTIVE;
        action_state.frame_count = 0;
        current_actions_.insert_or_assign(action, action_state);

        if (!isActionEnabled(action)) {
            LOGV("Action disabled: %s", string_action_map.at(action).c_str());
        }
    }

    if (!isActionEnabled(action)) {
        return;
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
    std::shared_ptr<Actions> actions = std::make_shared<Actions>(components);

    actions->reloadFromJson(j);
    return actions;
}

void Actions::reloadFromJson(nlohmann::json /* j */, File /* file_instance */) {
    // Enable everything by default
    setAllEnabled(true);
}

std::optional<nlohmann::json> Actions::outputToJson() {
    nlohmann::json j;

    return j;
}

void Actions::enableAction(Action action, bool enable) {
    enabled_actions_[static_cast<unsigned long long>(action)] = enable;
}

bool Actions::isActionEnabled(Action action) {
    return enabled_actions_[static_cast<unsigned long long>(action)];
}

void Actions::setAllEnabled(bool enabled) {
    for (auto& it : enabled_actions_) {
        it = enabled;
    }
}
