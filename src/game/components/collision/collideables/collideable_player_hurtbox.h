#pragma once

#include "components/collision/collision.h"

class CollideablePlayerHurtbox : public Collideable {
  public:
    CollideablePlayerHurtbox(std::weak_ptr<ComponentStore> components);

    CollisionType getType() const override;
};

