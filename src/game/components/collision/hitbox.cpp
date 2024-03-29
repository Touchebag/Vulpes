#include "hitbox.h"

Hitbox::Hitbox(unsigned int width, unsigned int height) :
    width_(width),
    height_(height) {
}

void Hitbox::setHitbox(unsigned int width, unsigned int height) {
    width_ = width;
    height_ = height;
}

void Hitbox::setOffset(std::pair<int, int> offset) {
    offset_ = offset;
}

std::pair<int, int> Hitbox::getOffset() {
    return {offset_.first * direction_multiplier_, offset_.second};
}

void Hitbox::moveOffset(std::pair<int, int> offset) {
    offset_.first += offset.first;
    offset_.second += offset.second;
}

int Hitbox::getRight() const {
    return (offset_.first * direction_multiplier_) + static_cast<int>(width_ / 2);
}

int Hitbox::getLeft() const {
    return (offset_.first * direction_multiplier_) - static_cast<int>(width_ / 2);
}

int Hitbox::getTop() const {
    return offset_.second - static_cast<int>(height_ / 2);
}

int Hitbox::getBottom() const {
    return offset_.second + static_cast<int>(height_ / 2);
}
