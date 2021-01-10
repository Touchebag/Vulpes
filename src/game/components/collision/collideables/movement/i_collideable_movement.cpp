#include "i_collideable_movement.h"

ICollideableMovement::ICollideableMovement(std::weak_ptr<Transform> trans) :
    Collideable(trans) {
}
