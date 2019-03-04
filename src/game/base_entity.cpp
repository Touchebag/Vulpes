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

    return true;
}

void BaseEntity::setPosiition(util::X abs_x, util::Y abs_y) {
    trans_.setPosition(abs_x, abs_y);
}

void BaseEntity::setHitbox(util::Right right, util::Left left, util::Top top, util::Bottom bottom) {
    hitbox_.setHitbox(right, left, top, bottom);
    sprite_.setOrigin(static_cast<float>((right - left) / 2.0), static_cast<float>((bottom - top) / 2.0));
    sprite_.setTextureRect(sf::IntRect(0, 0, right - left, bottom - top));
}

Hitbox BaseEntity::getAbsHitbox() {
    Hitbox abs_hitbox;
    auto right = util::Right(hitbox_.right_ + trans_.getX());
    auto left = util::Left(hitbox_.left_ + trans_.getX());
    auto top = util::Top(hitbox_.top_ + trans_.getY());
    auto bottom = util::Bottom(hitbox_.bottom_ + trans_.getY());
    abs_hitbox.setHitbox(right, left, top, bottom);
    return abs_hitbox;
}

void BaseEntity::loadFromJson(nlohmann::json j) {
    // TODO Error handling
    setPosiition(util::X(j["position"]["x"].get<int>()),
                 util::Y(j["position"]["y"].get<int>()));

    auto right = util::Right(j["hitbox"]["right"].get<int>());
    auto left = util::Left(j["hitbox"]["left"].get<int>());
    auto top = util::Top(j["hitbox"]["top"].get<int>());
    auto bottom = util::Bottom(j["hitbox"]["bottom"].get<int>());

    loadTexture(j["sprite"].get<std::string>());

    setHitbox(right, left, top, bottom);
}

void BaseEntity::setTextureCoords(std::pair<util::Point, util::Point> rect) {
    sprite_.setTextureRect(sf::IntRect(rect.first.x, rect.first.y, rect.second.x, rect.second.y));
    sprite_.setOrigin(static_cast<float>(rect.second.x / 2.0), static_cast<float>(rect.second.y / 2.0));

    // Mirror sprites facing left
    auto mirror_scale = facing_right_ ? 1.0 : -1.0;
    // Scale to 100px, keep aspect ratio
    sprite_.setScale(static_cast<float>(mirror_scale * (200.0 / rect.second.y)), static_cast<float>(200.0 / rect.second.y));
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

