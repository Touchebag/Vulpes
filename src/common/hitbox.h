#pragma once

#include <tuple>

#include "utils/common.h"

class Hitbox {
  public:
    void setHitbox(util::X width, util::Y height);

    bool collides(Hitbox& hitbox);

    // This assumes that the objects have already been checked for rough collision
    std::pair<util::X, util::Y> getMaximumMovement(util::X stepX, util::Y stepY, Hitbox otherHitbox);

    void setOffset(std::pair<util::X, util::Y> offset);
    void moveOffset(std::pair<util::X, util::Y> offset);

    util::X width_ = util::X(0.0);
    util::Y height_ = util::Y(0.0);

  private:
    bool collidesX(Hitbox& otherHitbox);
    bool collidesY(Hitbox& otherHitbox);

    std::pair<util::X, util::Y> offset_ = {util::X(0), util::Y(0)};

    int getRight();
    int getLeft();
    int getTop();
    int getBottom();
};
