#include "i_collision_movement.h"

ICollisionMovement::ICollisionMovement(std::weak_ptr<Transform> trans) :
    Collision(trans) {
}
