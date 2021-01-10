#include "collideable_health.h"

CollideableHealth::CollideableHealth(std::weak_ptr<Transform> trans) :
    Collideable(trans) {
}

void CollideableHealth::reloadFromJson(nlohmann::json j) {
    Collideable::reloadFromJson(j);

    if (!j.contains("health")) {
        return;
    }

    health_ = j["health"].get<int>();
}

std::optional<nlohmann::json> CollideableHealth::outputToJson() {
    nlohmann::json j = Collideable::outputToJson().value();

    j["health"] = health_;

    return j;
}

int CollideableHealth::getHealth() const {
    return health_;
}

Collideable::CollisionType CollideableHealth::getType() const {
    return Collideable::CollisionType::HEALTH;
}
