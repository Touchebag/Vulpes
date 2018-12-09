#include "transform.h"

void Transform::setPosition(float x, float y) {
    pos_x_ = x;
    pos_y_ = y;
}

void Transform::move(float x, float y) {
    pos_x_ += x;
    pos_y_ += y;
}

float Transform::getX() {
    return pos_x_;
}

float Transform::getY() {
    return pos_y_;
}
