#pragma once

#include <tuple>

#include "utils/common.h"
#include "json.hpp"

class Hitbox {
  public:
    void setHitbox(int width, int height);

    void loadFromJson(nlohmann::json& j);

    std::optional<nlohmann::json> outputToJson();

    bool collides(Hitbox& hitbox);

    // This assumes that the objects have already been checked for rough collision
    std::pair<double, double> getMaximumMovement(double stepX, double stepY, Hitbox otherHitbox);

    void setOffset(std::pair<int, int> offset);
    void moveOffset(std::pair<int, int> offset);

    int width_ = 0;
    int height_ = 0;

  private:
    bool collidesX(Hitbox& otherHitbox);
    bool collidesY(Hitbox& otherHitbox);

    std::pair<int, int> offset_ = {0, 0};

    int getRight();
    int getLeft();
    int getTop();
    int getBottom();
};
