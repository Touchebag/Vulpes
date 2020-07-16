#include "actions.h"

#include "actions_player.h"

#include "common.h"
#include "utils/log.h"

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
            return;
        } else {
            LOGW("Missing death component");
        }
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

std::shared_ptr<Actions> Actions::createFromJson(nlohmann::json j, std::weak_ptr<Death> death, std::weak_ptr<Collision> coll) {
    if (j.contains("type") && j["type"].get<std::string>() == "player") {
        std::shared_ptr<ActionsPlayer> actions_player = std::make_shared<ActionsPlayer>(death, coll);
        actions_player->reloadFromJson(j);
        return actions_player;
    } else {
        std::shared_ptr<Actions> actions = std::make_shared<Actions>(death);
        actions->reloadFromJson(j);
        return actions;
    }
}

void Actions::reloadFromJson(nlohmann::json /* j */) {
}

std::optional<nlohmann::json> Actions::outputToJson() {
    nlohmann::json j;

    return j;
}
