#include "collideable_player_hitbox.h"

#include "components/collision/attack_attributes.h"

CollideablePlayerHitbox::CollideablePlayerHitbox(std::weak_ptr<Transform> trans) :
    CollideableDamage(trans) {
}

void CollideablePlayerHitbox::reloadFromJson(nlohmann::json j) {
    if (!j.contains("type") || !(j["type"].get<std::string>() == "player_hitbox")) {
        throw std::invalid_argument("Player hitbox invalid type" + j["type"].get<std::string>());
    }

    Collideable::reloadFromJson(j);

    if (!j.contains("attack")) {
        return;
    }

    attack_attributes_ = collision::parseAttackAttributes(j["attack"]);
}

std::optional<nlohmann::json> CollideablePlayerHitbox::outputToJson() {
    nlohmann::json j = Collideable::outputToJson().value();

    j["attack"] = collision::dumpAttackAttributes(attack_attributes_);

    return j;
}

Collideable::CollisionType CollideablePlayerHitbox::getType() const {
    return Collideable::CollisionType::PLAYER_HITBOX;
}
