#pragma once

#include "components/collision/collision.h"

class CollideablePlayerHurtbox : public Collideable {
  public:
    CollideablePlayerHurtbox(std::weak_ptr<Transform> trans);

    CollisionType getType() const override;
};

