#pragma once

#include <tuple>

#include "util.h"

class Hitbox {
  public:
    void setHitbox(float right, float left, float top, float bottom);

    bool collides(const Hitbox& hitbox);

    // This assumes that the objects have already been checked for rough collision
    std::tuple<util::X, util::Y> getMaximumMovement(util::X stepX, util::Y stepY, Hitbox otherHitbox);

    double right_ = 0.0;
    double left_ = 0.0;
    double top_ = 0.0;
    double bottom_ = 0.0;

  private:
    bool collidesX(const Hitbox& otherHitbox);
    bool collidesY(const Hitbox& otherHitbox);
};
