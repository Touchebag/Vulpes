#include "collideable_health.h"

CollisionHealth::CollisionHealth(std::weak_ptr<Transform> trans) :
    Collision(trans) {
}

void CollisionHealth::reloadFromJson(nlohmann::json j) {
    Collision::reloadFromJson(j);

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

int CollisionHealth::getHealth() const {
    return health_;
}

Collision::CollisionType CollisionHealth::getType() const {
    return Collision::CollisionType::HEALTH;
}
