#pragma once

class Hitbox {
  public:
    setHitbox(float right, float left, float top, float bottom);

    bool collides(Hitbox hitbox);

    float right_ = 0.0;
    float left_ = 0.0;
    float top_ = 0.0;
    float bottom_ = 0.0;
};
