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

void Transform::setPosition(util::Point point) {
    pos_x_ = point.x;
    pos_y_ = point.y;
}

util::Point Transform::getPosition() {
    return {getX(), getY()};
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
    return pos_x_;
}

int Transform::getY() const {
    return pos_y_;
}
