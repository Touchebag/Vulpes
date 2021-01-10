#pragma once

#include "components/collision/collideable.h"

class ICollideableMovement : public Collideable {
  public:
    ICollideableMovement(std::weak_ptr<Transform> trans);

    // Return how far other_coll could move with regards to this collision
    virtual std::pair<double, double> getMaximumMovement(double stepX, double stepY, std::shared_ptr<const Collideable> other_coll) const = 0;
};
