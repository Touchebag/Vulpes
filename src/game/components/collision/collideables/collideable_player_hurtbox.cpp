#include "collideable_player_hurtbox.h"

CollisionPlayerHurtbox::CollisionPlayerHurtbox(std::weak_ptr<Transform> trans) :
    Collision(trans) {
}

Collision::CollisionType CollisionPlayerHurtbox::getType() const {
    return Collision::CollisionType::PLAYER_HURTBOX;
}
