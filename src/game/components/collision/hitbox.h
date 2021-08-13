#pragma once

#include <tuple>

#include "utils/common.h"
#include "nlohmann/json.hpp"

class Hitbox {
  public:
    Hitbox(unsigned int width, unsigned int height);

    void setHitbox(unsigned int width, unsigned int height);

    void setOffset(std::pair<int, int> offset);
    std::pair<int, int> getOffset();
    void moveOffset(std::pair<int, int> offset);

    unsigned int width_ = 0;
    unsigned int height_ = 0;

    int getRight() const;
    int getLeft() const;
    int getTop() const;
    int getBottom() const;

    int direction_multiplier_ = 1;

  private:
    std::pair<int, int> offset_ = {0, 0};
};
