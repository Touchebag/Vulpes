#include "transform.h"

void Transform::setPosition(util::X x, util::Y y) {
    pos_x_ = x;
    pos_y_ = y;
}

void Transform::move(util::X x, util::Y y) {
    pos_x_ += x;
    pos_y_ += y;
}

util::X Transform::getX() {
    return pos_x_;
}

util::Y Transform::getY() {
    return pos_y_;
}
