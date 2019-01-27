#pragma once

#include "utils/common.h"

class MovableEntity {
  public:
    virtual void move(util::X velX, util::Y velY);

  protected:
    util::X velX_ = util::X(0.0);
    util::Y velY_ = util::Y(0.0);
};
