#pragma once

#include "components/collision/collision.h"

class ICollisionMovement : public Collision {
  public:
    ICollisionMovement(std::weak_ptr<Transform> trans);

    // Return how far other_coll could move with regards to this collision
    virtual std::pair<double, double> getMaximumMovement(double stepX, double stepY, std::shared_ptr<const Collision> other_coll) const = 0;
};
