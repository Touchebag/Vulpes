#include "collision_static.h"

CollisionStatic::CollisionStatic(std::weak_ptr<Transform> trans) :
    Collision(trans) {
}

Collision::CollisionType CollisionStatic::getType() const {
    return Collision::CollisionType::STATIC;
}
