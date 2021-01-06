#pragma once

#include "components/collision/collision.h"

class CollisionPlayerHurtbox : public Collision {
  public:
    CollisionPlayerHurtbox(std::weak_ptr<Transform> trans);

    CollisionType getType() const override;
};

