#pragma once

#include "utils/common.h"

class MovableEntity {
  public:
    virtual void move(util::X velX, util::Y velY);

  protected:
    util::X velx_ = util::X(0.0);
    util::Y vely_ = util::Y(0.0);
};
