#include "collideable_damage.h"

CollideableDamage::CollideableDamage(std::weak_ptr<ComponentStore> components) :
    Collideable(components) {
}

const collision::AttackAttributes CollideableDamage::getAttributes() const {
    return attack_attributes_;
}
