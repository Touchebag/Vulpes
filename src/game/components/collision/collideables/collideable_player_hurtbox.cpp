#include "collideable_player_hurtbox.h"

#include "components/collision/collideables/collideable_collectible.h"
#include "components/actions/common.h"
#include "system/system.h"

#include "utils/log.h"

CollideablePlayerHurtbox::CollideablePlayerHurtbox(std::weak_ptr<ComponentStore> components) :
    Collideable(components) {
}

Collideable::CollisionType CollideablePlayerHurtbox::getType() const {
    return Collideable::CollisionType::PLAYER_HURTBOX;
}
