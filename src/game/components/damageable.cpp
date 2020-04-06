#include "damageable.h"

#include "utils/log.h"
#include "system/world.h"

Damageable::Damageable(std::weak_ptr<Collision> hurtbox) :
    hurtbox_(hurtbox) {
}

void Damageable::reloadFromJson(nlohmann::json j) {
    if (j.contains("health")) {
        health_ = j["health"].get<int>();
    }
}

std::optional<nlohmann::json> Damageable::outputToJson() {
    return {};
}

void Damageable::update() {
    // TODO Make CollisionType member

    if (auto coll = hurtbox_.lock()) {
        for (auto& it : World::IWorldRead::getCollisions(Collision::CollisionType::ENEMY_HITBOX)) {
            if (auto other_coll = it.lock()) {
                if (coll->collides(other_coll)) {
                    health_ -= 1;
                }
            }
        }
    }
}

int Damageable::getHealth() {
    return health_;
}

bool Damageable::isDead() {
    return health_ <= 0;
}
