#include "damageable.h"

#include "utils/log.h"
#include "system/world.h"

namespace {

typedef Collision::CollisionType Ctype;

std::map<Ctype, Ctype> type_mapping = {
    {Ctype::PLAYER_HURTBOX, Ctype::ENEMY_HITBOX},
    {Ctype::ENEMY_HITBOX, Ctype::PLAYER_HITBOX}
};

} // namespace

Damageable::Damageable(std::weak_ptr<Collision> hurtbox, bool& active) :
    hurtbox_(hurtbox),
    active_(active) {
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
    if (auto coll = hurtbox_.lock()) {
        auto hurting_type = type_mapping.at(coll->getType());
        for (auto& it : World::IWorldRead::getCollisions(hurting_type)) {
            if (auto other_coll = it.lock()) {
                if (coll->collides(other_coll)) {
                    health_ -= 1;
                    if (isDead()) {
                        active_ = false;
                    }
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
