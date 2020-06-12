#pragma once

#include <tuple>

#include "utils/common.h"
#include "json.hpp"

class Hitbox {
  public:
    Hitbox(int width, int height);

    void setHitbox(int width, int height);

    void setOffset(std::pair<int, int> offset);
    void moveOffset(std::pair<int, int> offset);

    int width_ = 0;
    int height_ = 0;

    int getRight() const;
    int getLeft() const;
    int getTop() const;
    int getBottom() const;

  private:
    std::pair<int, int> offset_ = {0, 0};
};