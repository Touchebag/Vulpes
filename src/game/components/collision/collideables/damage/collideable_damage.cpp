#include "collideable_damage.h"

CollideableDamage::CollideableDamage(std::weak_ptr<Transform> trans) :
    Collideable(trans) {
}

const collision::AttackAttributes CollideableDamage::getAttributes() const {
    return attack_attributes_;
}
