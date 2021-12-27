#include "transform.h"

Transform::Transform(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void Transform::update() {
}

void Transform::setPosition(const int x, const int y) {
    pos_x_ = x;
    pos_y_ = y;
}

void Transform::setPosition(util::Vec2i point) {
    pos_x_ = static_cast<double>(point.x);
    pos_y_ = static_cast<double>(point.y);
}

void Transform::setRawPosition(util::Vec2d point) {
    pos_x_ = point.x;
    pos_y_ = point.y;
}

util::Vec2i Transform::getPosition() const {
    return {getX(), getY()};
}

util::Vec2d Transform::getRawPosition() const {
    return {pos_x_, pos_y_};
}

std::shared_ptr<Transform> Transform::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<Transform>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void Transform::reloadFromJson(nlohmann::json j, File /* file_instance */) {
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
    return static_cast<int>(lround(pos_x_));
}

int Transform::getY() const {
    return static_cast<int>(lround(pos_y_));
}
