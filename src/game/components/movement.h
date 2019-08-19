#pragma once

#include "utils/common.h"
#include "components/hitbox.h"
#include "components/transform.h"
#include "components/collision.h"

#include <utility>
#include <memory>

class MovableEntity {
  public:
    MovableEntity(std::weak_ptr<Transform> trans, std::weak_ptr<Hitbox> hbox, std::weak_ptr<Collision> collision);

    void move(double velX, double velY);

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

    std::pair<double, double> getMaximumMovement(double velX, double velY);

    double getVelX();
    double getVelY();

  private:
    double velx_ = 0.0;
    double vely_ = 0.0;

    std::weak_ptr<Transform> trans_;
    std::weak_ptr<Hitbox> hbox_;
    std::weak_ptr<Collision> collision_;
};
