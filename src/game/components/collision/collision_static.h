#pragma once

#include "collision.h"

class CollisionStatic : public Collision {
  public:
    CollisionStatic(std::weak_ptr<Transform> trans);

    CollisionType getType() const override;
};

