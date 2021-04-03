#pragma once

#include "i_collideable_movement.h"

class CollideableSemiSolid : public ICollideableMovement {
  public:
    CollideableSemiSolid(std::weak_ptr<ComponentStore> components);

    CollisionType getType() const override;

    // Return how far other_coll could move with regards to this collision
    std::pair<double, double> getMaximumMovement(double stepX, double stepY, std::shared_ptr<const Collideable> other_coll) const override;
};

