#pragma once

#include "components/collision/collideables/movement/i_collideable_movement.h"

class CollideableSlope : public ICollideableMovement {
  public:
    explicit CollideableSlope(std::weak_ptr<ComponentStore> components);

    CollisionType getType() const override;

    void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    // Return how far other_coll could move with regards to this collision
    std::pair<double, double> getMaximumMovement(double stepX, double stepY, std::shared_ptr<const Collideable> other_coll) const override;

  private:
    void recalculateConstants();

    int getCurrentHeight(int x) const;

    double slope_coeff_ = 0.0;
    int slope_const_ = 0;

    bool direction_right_ = true;
};

