#include "collision_player_hitbox.h"

#include "components/collision/attack_attributes.h"

CollisionPlayerHitbox::CollisionPlayerHitbox(std::weak_ptr<Transform> trans) :
    CollisionDamage(trans) {
}

void CollisionPlayerHitbox::reloadFromJson(nlohmann::json j) {
    if (!j.contains("type") || !(j["type"].get<std::string>() == "player_hitbox")) {
        throw std::invalid_argument("Player hitbox invalid type" + j["type"].get<std::string>());
    }

    Collision::reloadFromJson(j);

    if (!j.contains("attack")) {
        return;
    }

    attack_attributes_ = collision::parseAttackAttributes(j["attack"]);
}

std::optional<nlohmann::json> CollisionPlayerHitbox::outputToJson() {
    nlohmann::json j = Collision::outputToJson().value();

    j["attack"] = collision::dumpAttackAttributes(attack_attributes_);

    return j;
}

Collision::CollisionType CollisionPlayerHitbox::getType() const {
    return Collision::CollisionType::PLAYER_HITBOX;
}
