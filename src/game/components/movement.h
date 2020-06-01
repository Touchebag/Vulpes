#pragma once

#include "utils/common.h"
#include "components/transform.h"
#include "components/collision/collision.h"

#include <utility>
#include <memory>

class MovableEntity : public Component {
  public:
    MovableEntity(std::weak_ptr<Transform> trans, std::weak_ptr<Collision> collision);

    void move(double velX, double velY);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    std::pair<double, double> getMaximumMovement(double velX, double velY);

    double getVelX();
    double getVelY();

    bool facing_right_ = true;

  private:
    double velx_ = 0.0;
    double vely_ = 0.0;

    std::weak_ptr<Transform> trans_;
    std::weak_ptr<Hitbox> hbox_;
    std::weak_ptr<Collision> collision_;
};
