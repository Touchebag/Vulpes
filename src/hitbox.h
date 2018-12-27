#pragma once

#include <tuple>

#include "util.h"

class Hitbox {
  public:
    void setHitbox(util::Right right, util::Left left, util::Top top, util::Bottom bottom);

    bool collides(Hitbox& hitbox);

    // This assumes that the objects have already been checked for rough collision
    std::tuple<util::X, util::Y> getMaximumMovement(util::X stepX, util::Y stepY, Hitbox otherHitbox);

    util::Right right_ = util::Right(0.0);
    util::Left left_ = util::Left(0.0);
    util::Top top_ = util::Top(0.0);
    util::Bottom bottom_ = util::Bottom(0.0);

  private:
    bool collidesX(Hitbox& otherHitbox);
    bool collidesY(Hitbox& otherHitbox);
};
