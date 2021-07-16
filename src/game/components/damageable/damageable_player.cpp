#include "damageable_player.h"

#include "system/system.h"
#include "components/collision/collideables/collideable_collectible.h"

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

    if (auto coll = getComponent<Collision>()) {
        for (auto& it : System::IWorldRead::getCollideables(Collideable::CollisionType::COLLECTIBLE)) {
            if (auto other_coll = it.lock()) {
                if (coll->collides(other_coll)) {
                    if (auto health_collision = std::dynamic_pointer_cast<const CollideableCollectible>(other_coll)) {
                        health_ += health_collision->getHealth();
                    }
                }
            }
        }
    }
}
