#include "collideable_hitbox.h"

CollideableHitbox::CollideableHitbox(std::weak_ptr<ComponentStore> components) :
    Collideable(components) {
}

void CollideableHitbox::reloadFromJson(nlohmann::json j) {
    Collideable::reloadFromJson(j);

    if (j.contains("attack")) {
        attack_attributes_ = collision::parseAttackAttributes(j["attack"]);
    }
}

std::optional<nlohmann::json> CollideableHitbox::outputToJson() {
    if (auto j = Collideable::outputToJson()) {
        auto j_val = j.value();

        j_val["attack"] = collision::dumpAttackAttributes(attack_attributes_);

        return j_val;
    } else {
        return std::nullopt;
    }
}

Collideable::CollisionType CollideableHitbox::getType() const {
    return Collideable::CollisionType::HITBOX;
}

const collision::AttackAttributes CollideableHitbox::getAttributes() const {
    return attack_attributes_;
}
