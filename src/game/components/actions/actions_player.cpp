#include "actions_player.h"

#include <unordered_map>

#include "common.h"
#include "system/world.h"
#include "utils/log.h"

#include "components/collision/collision_collectible.h"
#include "components/collision/collision_interactable.h"

namespace {

std::unordered_map<int, Actions::Action> id_actions_map = {
    {0, Actions::Action::JUMP},
    {1, Actions::Action::DASH},
    {2, Actions::Action::ATTACK},
};

} // namespace

ActionsPlayer::ActionsPlayer(std::weak_ptr<Death> death, std::weak_ptr<Collision> coll, std::weak_ptr<StatefulEntity> state) :
    Actions(death),
    coll_(coll),
    state_(state) {
}

void ActionsPlayer::update() {
    if (auto coll = coll_.lock()) {
        for (auto& it : World::IWorldRead::getCollisions(Collision::CollisionType::COLLECTIBLE)) {
            if (auto other_coll = it.lock()) {
                if (coll->collides(other_coll)) {
                    if (auto collectible = std::dynamic_pointer_cast<const CollisionCollectible>(other_coll)) {
                        enableAction(id_actions_map.at(collectible->getId()));
                    }
                }
            }
        }

        if (getActionState(Action::INTERACT, true)) {
            World::IWorldModify::triggerInterract();
        }
    }

    Actions::update();
}

void ActionsPlayer::enableAction(Action action) {
    enabled_actions_[static_cast<int>(action)] = true;
}

bool ActionsPlayer::isActionEnabled(Action action) {
    return enabled_actions_[static_cast<int>(action)];
}

void ActionsPlayer::addAction(Actions::Action action) {
    if (action == Action::INTERACT) {
        if (auto state = state_.lock()) {
            if (!state->getStateProperties().can_interact) {
                return;
            }
        }
    }

    int action_number = static_cast<int>(action);

    if (action_number < static_cast<int>(Action::NUM_ACTIONS) && !isActionEnabled(action)) {
        LOGD("Action disabled: %s", string_action_map.at(action).c_str());
        return;
    }

    Actions::addAction(action);
}

void ActionsPlayer::reloadFromJson(nlohmann::json j) {
    if (j.contains("movement_x")) {
        enableAction(Action::MOVE_RIGHT);
        enableAction(Action::MOVE_LEFT);
    }

    if (j.contains(string_action_map.at(Action::JUMP))) {
        enableAction(Action::JUMP);
    }

    if (j.contains(string_action_map.at(Action::DASH))) {
        enableAction(Action::DASH);
    }

    if (j.contains(string_action_map.at(Action::ATTACK))) {
        enableAction(Action::ATTACK);
    }

    if (j.contains(string_action_map.at(Action::INTERACT))) {
        enableAction(Action::INTERACT);
    }
}

std::optional<nlohmann::json> ActionsPlayer::outputToJson() {
    nlohmann::json j = Actions::outputToJson().value();

    j["type"] = "player";

    if (isActionEnabled(Action::MOVE_RIGHT) &&
        isActionEnabled(Action::MOVE_LEFT)) {
        j["movement_x"] = true;
    }

    if (isActionEnabled(Action::JUMP)) {
        j[string_action_map.at(Action::JUMP)] = true;
    }

    if (isActionEnabled(Action::DASH)) {
        j[string_action_map.at(Action::DASH)] = true;
    }

    if (isActionEnabled(Action::ATTACK)) {
        j[string_action_map.at(Action::ATTACK)] = true;
    }

    if (isActionEnabled(Action::INTERACT)) {
        j[string_action_map.at(Action::INTERACT)] = true;
    }

    return j;
}
