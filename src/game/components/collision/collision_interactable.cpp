#include "collision_interactable.h"

#include "utils/log.h"

CollisionInteractable::CollisionInteractable(std::weak_ptr<Transform> trans) :
    Collision(trans) {
}

Collision::CollisionType CollisionInteractable::getType() const {
    return Collision::CollisionType::INTERACTABLE;
}
