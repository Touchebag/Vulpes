#include "collideable_enemy_hitbox.h"

#include "components/collision/attack_attributes.h"

CollideableEnemyHitbox::CollideableEnemyHitbox(std::weak_ptr<ComponentStore> components) :
    CollideableDamage(components) {
}

void CollideableEnemyHitbox::reloadFromJson(nlohmann::json j) {
    Collideable::reloadFromJson(j);

    if (!j.contains("attack")) {
        return;
    }

    attack_attributes_ = collision::parseAttackAttributes(j["attack"]);
}

std::optional<nlohmann::json> CollideableEnemyHitbox::outputToJson() {
    nlohmann::json j = Collideable::outputToJson().value();

    j["attack"] = collision::dumpAttackAttributes(attack_attributes_);

    return j;
}

Collideable::CollisionType CollideableEnemyHitbox::getType() const {
    return CollisionType::ENEMY_HITBOX;
}
