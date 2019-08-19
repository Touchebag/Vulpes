#pragma once

#include <memory>

#include "components/transform.h"
#include "components/hitbox.h"

class Collision {
  public:
    Collision(std::weak_ptr<Transform> trans, std::weak_ptr<Hitbox> hbox);

    void loadFromJson(nlohmann::json& j);
    std::optional<nlohmann::json> outputToJson();

    bool collides(std::weak_ptr<Collision> other_entity);

    std::pair<double, double> getMaximumMovement(double stepX, double stepY,
            std::shared_ptr<Transform> other_trans, std::shared_ptr<Hitbox> other_hbox);

  private:
    std::weak_ptr<Transform> trans_;
    std::weak_ptr<Hitbox> hbox_;
};
