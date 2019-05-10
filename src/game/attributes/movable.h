#pragma once

#include "utils/common.h"
#include "hitbox.h"
#include "transform.h"

#include <utility>

class MovableEntity {
  public:
    virtual void move(util::X velX, util::Y velY, Transform& trans) final;

    virtual std::pair<util::X, util::Y> getMaximumMovement(util::X velX, util::Y velY, Hitbox abs_hitbox) final;

  protected:
    util::X velx_ = util::X(0.0);
    util::Y vely_ = util::Y(0.0);
};
