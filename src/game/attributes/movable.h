#pragma once

#include "utils/common.h"
#include "hitbox.h"
#include "transform.h"

#include <utility>
#include <memory>

class MovableEntity {
  public:
    MovableEntity(std::weak_ptr<Transform> trans, std::weak_ptr<Hitbox> hbox);

    void move(double velX, double velY);

    std::pair<double, double> getMaximumMovement(double velX, double velY, Hitbox abs_hitbox);

    double getVelX();
    double getVelY();

  private:
    double velx_ = 0.0;
    double vely_ = 0.0;

    std::weak_ptr<Transform> trans_;
    std::weak_ptr<Hitbox> hbox_;
};
