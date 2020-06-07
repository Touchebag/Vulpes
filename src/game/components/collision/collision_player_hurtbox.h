#pragma once

#include "collision.h"

class CollisionPlayerHurtbox : public Collision {
  public:
    CollisionPlayerHurtbox(std::weak_ptr<Transform> trans);

    CollisionType getType() const override;
};

