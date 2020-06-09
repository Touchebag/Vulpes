#include "collision_enemy_hitbox.h"

CollisionEnemyHitbox::CollisionEnemyHitbox(std::weak_ptr<Transform> trans) :
    Collision(trans) {
}

void CollisionEnemyHitbox::reloadFromJson(nlohmann::json j) {
    Collision::reloadFromJson(j);

    if (!j.contains("attack")) {
        return;
    }

    attack_attributes_ = collision::parseAttackAttributes(j["attack"]);
}

std::optional<nlohmann::json> CollisionEnemyHitbox::outputToJson() {
    nlohmann::json j = Collision::outputToJson().value();

    j["attack"] = collision::dumpAttackAttributes(attack_attributes_);

    return j;
}

Collision::CollisionType CollisionEnemyHitbox::getType() const {
    return Collision::CollisionType::ENEMY_HITBOX;
}

const collision::AttackAttributes CollisionEnemyHitbox::getAttributes() const {
    return attack_attributes_;
}
