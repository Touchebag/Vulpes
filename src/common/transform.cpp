#include "transform.h"

void Transform::setPosition(int x, int y) {
    pos_x_ = x;
    pos_y_ = y;
}

void Transform::move(int x, int y) {
    pos_x_ += x;
    pos_y_ += y;
}

int Transform::getX() {
    return pos_x_;
}

int Transform::getY() {
    return pos_y_;
}
