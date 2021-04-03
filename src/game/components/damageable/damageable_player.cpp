#include "damageable_player.h"

#include "system/world.h"
#include "components/collision/collideables/collideable_health.h"

DamageablePlayer::DamageablePlayer(std::weak_ptr<ComponentStore> components) :
    Damageable(components) {
}

std::optional<nlohmann::json> DamageablePlayer::outputToJson() {
    auto j = Damageable::outputToJson();

    if (j) {
        j.value()["type"] = "player";
    }

    return j;
}

void DamageablePlayer::update() {
    Damageable::update();

    if (auto coll = component_store_.lock()->collision) {
        for (auto& it : World::IWorldRead::getCollideables(Collideable::CollisionType::HEALTH)) {
            if (auto other_coll = it.lock()) {
                if (coll->collides(other_coll)) {
                    if (auto health_collision = std::dynamic_pointer_cast<const CollideableHealth>(other_coll)) {
                        health_ += health_collision->getHealth();
                    }
                }
            }
        }
    }
}
