#pragma once

#include "utils/common.h"
#include "hitbox.h"
#include "transform.h"

#include <utility>

class MovableEntity {
  public:
    virtual void move(double velX, double velY, Transform& trans) final;

    virtual std::pair<double, double> getMaximumMovement(double velX, double velY, Hitbox abs_hitbox) final;

  protected:
    double velx_ = 0.0;
    double vely_ = 0.0;
};
