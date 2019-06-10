#pragma once

#include "utils/common.h"
#include "hitbox.h"
#include "transform.h"

#include <utility>

class MovableEntity {
  public:
    virtual void move(int velX, int velY, Transform& trans) final;

    virtual std::pair<int, int> getMaximumMovement(int velX, int velY, Hitbox abs_hitbox) final;

  protected:
    int velx_ = 0;
    int vely_ = 0;
};
