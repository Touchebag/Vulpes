#pragma once

#include "collision.h"

class CollisionSemiSolid : public Collision {
  public:
    CollisionSemiSolid(std::weak_ptr<Transform> trans);

    CollisionType getType() const override;

    // Return how far other_coll could move with regards to this collision
    std::pair<double, double> getMaximumMovement(double stepX, double stepY, std::shared_ptr<const Collision> other_coll) const;
};
