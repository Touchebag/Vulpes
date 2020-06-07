#include "collision_enemy_hitbox.h"

CollisionEnemyHitbox::CollisionEnemyHitbox(std::weak_ptr<Transform> trans) :
    Collision(trans) {
}

void CollisionEnemyHitbox::reloadFromJson(nlohmann::json j) {
    Collision::reloadFromJson(j);

    if (!j.contains("attack")) {
        return;
    }

    auto attack = j["attack"];

    if (attack.contains("damage")) {
        attack_attributes_.damage = attack["damage"];
    }

    if (attack.contains("knockback_x")) {
        attack_attributes_.knockback_x = attack["knockback_x"];
    }
    if (attack.contains("knockback_y")) {
        attack_attributes_.knockback_y = attack["knockback_y"];
    }
}

std::optional<nlohmann::json> CollisionEnemyHitbox::outputToJson() {
    nlohmann::json j = Collision::outputToJson().value();
    nlohmann::json attack;

    if (attack_attributes_.damage > 0) {
        attack["damage"] = attack_attributes_.damage;
    }

    if (attack_attributes_.knockback_x != 0) {
        attack["knockback_x"] = attack_attributes_.knockback_x;
    }
    if (attack_attributes_.knockback_y != 0) {
        attack["knockback_y"] = attack_attributes_.knockback_y;
    }

    j["attack"] = attack;

    return j;
}

Collision::CollisionType CollisionEnemyHitbox::getType() const {
    return Collision::CollisionType::ENEMY_HITBOX;
}

const collision::AttackAttributes CollisionEnemyHitbox::getAttributes() const {
    return attack_attributes_;
}
