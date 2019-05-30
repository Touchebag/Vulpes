#include "base_entity.h"

#include "log.h"

#include <unistd.h>

bool BaseEntity::loadTexture(std::string file_path) {
    LOGV("%s", file_path.c_str());
    if (!texture_.loadFromFile(std::string("assets/") + file_path.c_str())) {
        LOGW("Error loading image %s", file_path.c_str());
        return false;
    }

    sprite_.setTexture(texture_, true);
    texture_.setRepeated(true);
    texture_name_ = file_path;

    return true;
}

void BaseEntity::setPosition(util::X abs_x, util::Y abs_y) {
    trans_.setPosition(abs_x, abs_y);
}

sf::Vector2i BaseEntity::getPosition() {
    return {trans_.getX(), trans_.getY()};
}

void BaseEntity::setHitbox(util::X width, util::Y height) {
    hitbox_.setHitbox(width, height);
    sprite_.setOrigin(static_cast<float>(width / 2.0), static_cast<float>(height / 2.0));
    sprite_.setTextureRect(sf::IntRect(0, 0, width, height));
}

Hitbox BaseEntity::getHitbox() {
    return hitbox_;
}

Hitbox BaseEntity::getAbsHitbox() {
    Hitbox abs_hitbox;
    abs_hitbox.setHitbox(hitbox_.width_, hitbox_.height_);
    abs_hitbox.setOffset({util::X(trans_.getX()), util::Y(trans_.getY())});
    return abs_hitbox;
}

void BaseEntity::loadFromJson(nlohmann::json j) {
    // TODO Error handling
    setPosition(util::X(j["position"]["x"].get<int>()),
                util::Y(j["position"]["y"].get<int>()));

    auto width = util::X(j["hitbox"]["width"].get<int>());
    auto height = util::Y(j["hitbox"]["height"].get<int>());

    texture_name_ = j["sprite"].get<std::string>();
    loadTexture(texture_name_);

    setHitbox(width, height);
}

std::optional<nlohmann::json> BaseEntity::outputToJson() {
    nlohmann::json j;
    j["position"]["x"] = static_cast<int>(trans_.getX());
    j["position"]["y"] = static_cast<int>(trans_.getY());

    j["hitbox"]["width"] = static_cast<int>(hitbox_.width_);
    j["hitbox"]["height"] = static_cast<int>(hitbox_.height_);

    j["sprite"] = texture_name_;

    return {j};
}

void BaseEntity::setTextureCoords(std::pair<util::Point, util::Point> rect) {
    sprite_.setTextureRect(sf::IntRect(rect.first.x_, rect.first.y_, rect.second.x_, rect.second.y_));
    sprite_.setOrigin(static_cast<float>(rect.second.x_ / 2.0), static_cast<float>(rect.second.y_ / 2.0));

    // Mirror sprites facing left
    auto mirror_scale = facing_right_ ? 1.0 : -1.0;
    // Scale to 100px, keep aspect ratio
    sprite_.setScale(static_cast<float>(mirror_scale * (200.0 / rect.second.y_)), static_cast<float>(200.0 / rect.second.y_));
}

void BaseEntity::update() {
}

void BaseEntity::render(sf::RenderWindow& window) {
    sprite_.setPosition(static_cast<float>(trans_.getX()), static_cast<float>(trans_.getY()));
    window.draw(sprite_);

    // TODO Move to debug menu
    // sf::RectangleShape rectangle(sf::Vector2f(hitbox_.right_ - hitbox_.left_, hitbox_.bottom_ - hitbox_.top_));
    // rectangle.setPosition(trans_.getX() + hitbox_.left_, trans_.getY() + hitbox_.top_);
    // rectangle.setFillColor(sf::Color(255, 0, 0, 64));
    // window.draw(rectangle);
}

