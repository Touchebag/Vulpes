#include "collision_collectible.h"

CollisionCollectible::CollisionCollectible(std::weak_ptr<Transform> trans) :
    Collision(trans) {
}

void CollisionCollectible::reloadFromJson(nlohmann::json j) {
    Collision::reloadFromJson(j);

    if (j.contains("id")) {
        id_ = j["id"].get<int>();
    }
}

std::optional<nlohmann::json> CollisionCollectible::outputToJson() {
    nlohmann::json j = Collision::outputToJson().value();

    j["id"] = getId();

    return j;
}

Collision::CollisionType CollisionCollectible::getType() const {
    return CollisionType::COLLECTIBLE;
}

int CollisionCollectible::getId() const {
    return id_;
}
