#pragma once

#include <tuple>

class Hitbox {
  public:
    void setHitbox(float right, float left, float top, float bottom);

    bool collides(const Hitbox& hitbox);

    // This assumes that the objects have already been checked for rough collision
    std::tuple<float, float> getMaximumMovement(float stepX, float stepY, Hitbox otherHitbox);

    float right_ = 0.0;
    float left_ = 0.0;
    float top_ = 0.0;
    float bottom_ = 0.0;

  private:
    bool collidesX(const Hitbox& otherHitbox);
    bool collidesY(const Hitbox& otherHitbox);
};
