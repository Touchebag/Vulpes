#include "collideable_hurtbox.h"

#include "components/component_store.h"

CollideableHurtbox::CollideableHurtbox(std::weak_ptr<ComponentStore> components) :
    Collideable(components) {
}

Collideable::CollisionType CollideableHurtbox::getType() const {
    return Collideable::CollisionType::HURTBOX;
}
