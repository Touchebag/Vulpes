#pragma once

#include "collision.h"

class CollisionInteractable : public Collision {
  public:
    CollisionInteractable(std::weak_ptr<Transform> trans);

    Collision::CollisionType getType() const override;
};
