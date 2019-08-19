#include "hitbox.h"

void Hitbox::setHitbox(int width, int height) {
    width_ = width;
    height_ = height;
}

void Hitbox::loadFromJson(nlohmann::json& j) {
    width_ = j["width"].get<int>();
    height_= j["height"].get<int>();
}

std::optional<nlohmann::json> Hitbox::outputToJson() {
    nlohmann::json j;

    j["width"] = static_cast<int>(width_);
    j["height"] = static_cast<int>(height_);

    return {j};
}

void Hitbox::setOffset(std::pair<int, int> offset) {
    offset_ = offset;
}

void Hitbox::moveOffset(std::pair<int, int> offset) {
    offset_.first += offset.first;
    offset_.second += offset.second;
}

int Hitbox::getRight() const {
    return offset_.first + (width_ / 2);
}

int Hitbox::getLeft() const {
    return offset_.first - (width_ / 2);
}

int Hitbox::getTop() const {
    return offset_.second - (height_ / 2);
}

int Hitbox::getBottom() const {
    return offset_.second + (height_ / 2);
}
