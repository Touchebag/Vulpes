#include "actions_player.h"

#include <unordered_map>

#include "common.h"
#include "system/world.h"
#include "utils/log.h"

#include "components/collision/collideables/collideable_collectible.h"
#include "components/collision/collideables/collideable_interactable.h"

ActionsPlayer::ActionsPlayer(std::weak_ptr<Death> death) :
    Actions(death) {
}

void ActionsPlayer::update() {
    if (getActionState(Action::INTERACT, true)) {
        World::IWorldModify::triggerInterract();
    }

    Actions::update();
}

void ActionsPlayer::addAction(Actions::Action action) {
    Actions::addAction(action);
}

void ActionsPlayer::reloadFromJson(nlohmann::json j) {
    Actions::reloadFromJson(j);
}

std::optional<nlohmann::json> ActionsPlayer::outputToJson() {
    nlohmann::json j = Actions::outputToJson().value();

    j["type"] = "player";

    return j;
}
