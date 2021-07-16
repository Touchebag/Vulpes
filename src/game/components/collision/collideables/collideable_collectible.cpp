#include "collideable_collectible.h"

CollideableCollectible::CollideableCollectible(std::weak_ptr<ComponentStore> components) :
    Collideable(components) {
}

void CollideableCollectible::reloadFromJson(nlohmann::json j) {
    Collideable::reloadFromJson(j);

    if (j.contains("health")) {
        health_ = j["health"].get<int>();
    }
}

std::optional<nlohmann::json> CollideableCollectible::outputToJson() {
    nlohmann::json j = Collideable::outputToJson().value();

    if (health_ != 0) {
        j["health"] = health_;
    }

    return j;
}

int CollideableCollectible::getHealth() const {
    return health_;
}

Collideable::CollisionType CollideableCollectible::getType() const {
    return CollisionType::COLLECTIBLE;
}
