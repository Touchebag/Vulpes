#pragma once

#include "components/collision/collideable.h"

class ICollideableMovement : public Collideable {
  public:
    struct CollisionData {
        std::pair<int, int> normals;
        double collision_time;
    };

    explicit ICollideableMovement(std::weak_ptr<ComponentStore> components);

    // Return how far other_coll could move with regards to this collision
    virtual std::pair<double, double> getMaximumMovement(double stepX, double stepY, std::shared_ptr<const Collideable> other_coll) const = 0;

  protected:
    CollisionData calculateSweptCollision(std::weak_ptr<const Collideable> other_entity, const std::pair<double, double> velocity) const;
};
