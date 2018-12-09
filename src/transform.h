#pragma once

class Transform {
public:
    void setPosition(float x, float y);
    void move(float x, float y);

    float getX();
    float getY();

private:
    float pos_x_ = 0.0;
    float pos_y_ = 0.0;
};
