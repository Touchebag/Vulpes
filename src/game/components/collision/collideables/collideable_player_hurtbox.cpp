#include "collideable_player_hurtbox.h"

CollideablePlayerHurtbox::CollideablePlayerHurtbox(std::weak_ptr<Transform> trans) :
    Collideable(trans) {
}

Collideable::CollisionType CollideablePlayerHurtbox::getType() const {
    return Collideable::CollisionType::PLAYER_HURTBOX;
}
