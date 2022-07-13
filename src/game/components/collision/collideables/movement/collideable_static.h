#pragma once

#include <nlohmann/json.hpp>

#include "components/collision/collideables/movement/i_collideable_movement.h"

class CollideableStatic : public ICollideableMovement {
  public:
    explicit CollideableStatic(std::weak_ptr<ComponentStore> components);

    virtual void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;

    // Return how far other_coll could move with regards to this collision
    std::pair<double, double> getMaximumMovement(double stepX, double stepY, std::shared_ptr<const Collideable> other_coll) const override;

  private:
    bool solid_top_ = true;
    bool solid_bottom_ = true;
    bool solid_left_ = true;
    bool solid_right_ = true;
};

