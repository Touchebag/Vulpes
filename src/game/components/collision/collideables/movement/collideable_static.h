#pragma once

#include "components/collision/collideables/movement/i_collideable_movement.h"

class CollideableStatic : public ICollideableMovement {
  public:
    CollideableStatic(std::weak_ptr<ComponentStore> components);

    CollisionType getType() const override;

    // Return how far other_coll could move with regards to this collision
    std::pair<double, double> getMaximumMovement(double stepX, double stepY, std::shared_ptr<const Collideable> other_coll) const override;
};

