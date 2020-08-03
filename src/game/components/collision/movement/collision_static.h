#pragma once

#include "components/collision/movement/i_collision_movement.h"

class CollisionStatic : public ICollisionMovement {
  public:
    CollisionStatic(std::weak_ptr<Transform> trans);

    CollisionType getType() const override;

    // Return how far other_coll could move with regards to this collision
    std::pair<double, double> getMaximumMovement(double stepX, double stepY, std::shared_ptr<const Collision> other_coll) const override;
};

