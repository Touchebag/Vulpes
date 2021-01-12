#include "actions_player.h"

#include <unordered_map>

#include "common.h"
#include "system/world.h"
#include "utils/log.h"

#include "components/collision/collideables/collideable_collectible.h"
#include "components/collision/collideables/collideable_interactable.h"

namespace {

std::unordered_map<int, Actions::Action> id_actions_map = {
    {0, Actions::Action::JUMP},
    {1, Actions::Action::DASH},
    {2, Actions::Action::ATTACK},
};

} // namespace

ActionsPlayer::ActionsPlayer(std::weak_ptr<Death> death, std::weak_ptr<Collision> coll) :
    Actions(death),
    coll_(coll) {
}

void ActionsPlayer::update() {
    if (auto coll = coll_.lock()) {
        for (auto& it : World::IWorldRead::getCollideables(Collideable::CollisionType::COLLECTIBLE)) {
            if (auto other_coll = it.lock()) {
                if (coll->collides(other_coll)) {
                    if (auto collectible = std::dynamic_pointer_cast<const CollideableCollectible>(other_coll)) {
                        try {
                            enableAction(id_actions_map.at(collectible->getId()), true);
                        } catch (std::out_of_range& e) {
                            LOGE("Invalid action id %i", collectible->getId());
                            throw e;
                        }
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
