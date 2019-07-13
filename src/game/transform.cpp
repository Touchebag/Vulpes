#include "transform.h"

void Transform::setPosition(int x, int y) {
    pos_x_ = x;
    pos_y_ = y;
}

void Transform::move(int x, int y) {
    pos_x_ += x;
    pos_y_ += y;
}

void Transform::move(double x, double y) {
    move(static_cast<int>(x), static_cast<int>(y));
}

int Transform::getX() {
    return pos_x_;
}

int Transform::getY() {
    return pos_y_;
}
