#include "transform.h"

void Transform::setPosition(int x, int y) {
    pos_x_ = x;
    pos_y_ = y;
}

void Transform::loadFromJson(nlohmann::json j) {
    pos_x_ = j["pos_x"];
    pos_y_ = j["pos_y"];
}

std::optional<nlohmann::json> Transform::outputToJson() {
    nlohmann::json j;

    j["pos_x"] = pos_x_;
    j["pos_y"] = pos_y_;

    return {j};
}

int Transform::getX() const {
    return pos_x_;
}

int Transform::getY() const {
    return pos_y_;
}
