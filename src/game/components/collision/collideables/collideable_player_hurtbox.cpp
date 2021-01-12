#include "collideable_player_hurtbox.h"

#include "components/collision/collideables/collideable_collectible.h"
#include "components/actions/common.h"
#include "system/world.h"

#include "utils/log.h"

CollideablePlayerHurtbox::CollideablePlayerHurtbox(std::weak_ptr<Transform> trans, std::weak_ptr<Actions> actions) :
    Collideable(trans),
    actions_(actions) {
}

void CollideablePlayerHurtbox::update() {
    for (auto& it : World::IWorldRead::getCollideables(Collideable::CollisionType::COLLECTIBLE)) {
        if (auto other_coll = it.lock()) {
            if (collides(other_coll)) {
                if (auto collectible = std::dynamic_pointer_cast<const CollideableCollectible>(other_coll)) {
                    try {
                        if (auto actions = actions_.lock()) {
                            actions->enableAction(id_actions_map.at(collectible->getId()), true);
                        }
                    } catch (std::out_of_range& e) {
                        LOGE("Invalid action id %i", collectible->getId());
                        throw e;
                    }
                }
            }
        }
    }
}

Collideable::CollisionType CollideablePlayerHurtbox::getType() const {
    return Collideable::CollisionType::PLAYER_HURTBOX;
}
