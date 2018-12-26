#pragma once

#include "utility.h"

class MovableEntity {
  public:
    virtual void move(vec2d velocity);

  protected:
    vec2d velocity_;
};
