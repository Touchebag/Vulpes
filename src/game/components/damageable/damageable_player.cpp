#include "damageable_player.h"

#include "system/system.h"
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

    if (auto coll = getComponent<Collision>()) {
        for (auto& it : System::IWorldRead::getCollideables(Collideable::CollisionType::HEALTH)) {
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
