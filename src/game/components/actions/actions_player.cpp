#include "actions_player.h"

#include <unordered_map>

#include "common.h"
#include "system/system.h"
#include "utils/log.h"

#include "components/collision/collideables/collideable_collectible.h"
#include "components/collision/collideables/collideable_interactable.h"

ActionsPlayer::ActionsPlayer(std::weak_ptr<ComponentStore> components) :
    Actions(components) {
}

void ActionsPlayer::update() {
    if (getActionState(Action::INTERACT, true)) {
        System::IWorldModify::triggerInterract();
    }

    Actions::update();
}

void ActionsPlayer::addAction(Actions::Action action) {
    Actions::addAction(action);
}

void ActionsPlayer::reloadFromJson(nlohmann::json j, File file_instance) {
    Actions::reloadFromJson(j, file_instance);
}

std::optional<nlohmann::json> ActionsPlayer::outputToJson() {
    nlohmann::json j = Actions::outputToJson().value();

    j["type"] = "player";

    return j;
}
