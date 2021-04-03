#include "collideable_collectible.h"

CollideableCollectible::CollideableCollectible(std::weak_ptr<ComponentStore> components) :
    Collideable(components) {
}

void CollideableCollectible::reloadFromJson(nlohmann::json j) {
    Collideable::reloadFromJson(j);

    if (j.contains("id")) {
        id_ = j["id"].get<int>();
    }
}

std::optional<nlohmann::json> CollideableCollectible::outputToJson() {
    nlohmann::json j = Collideable::outputToJson().value();

    j["id"] = getId();

    return j;
}

Collideable::CollisionType CollideableCollectible::getType() const {
    return CollisionType::COLLECTIBLE;
}

int CollideableCollectible::getId() const {
    return id_;
}
