#include "collision_health.h"

#include "system/world.h"

CollisionHealth::CollisionHealth(std::weak_ptr<Transform> trans) :
    Collision(trans) {
}

void CollisionHealth::reloadFromJson(nlohmann::json j) {
    if (!j.contains("health")) {
        return;
    }

    health_ = j["health"].get<int>();
}

std::optional<nlohmann::json> CollisionHealth::outputToJson() {
    nlohmann::json j = Collision::outputToJson().value();

    j["health"] = health_;

    return j;
}

void CollisionHealth::update() {
    auto player = World::IWorldRead::getPlayer().lock();

    if (collides(player->collision_)) {
        player->damageable_->addHealth(health_);
    }
}

Collision::CollisionType CollisionHealth::getType() const {
    return Collision::CollisionType::HEALTH;
}
