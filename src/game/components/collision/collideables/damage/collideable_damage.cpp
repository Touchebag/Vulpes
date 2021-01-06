#include "collideable_damage.h"

CollisionDamage::CollisionDamage(std::weak_ptr<Transform> trans) :
    Collision(trans) {
}

const collision::AttackAttributes CollisionDamage::getAttributes() const {
    return attack_attributes_;
}
