#include "base_entity.h"

#include "log.h"

#include <unistd.h>

bool BaseEntity::loadTexture(std::string file_path) {
    LOGV("%s\n", file_path.c_str());
    if (!texture_.loadFromFile(file_path.c_str())) {
        LOGW("Error loading image %s", file_path.c_str());
        return false;
    }

    sprite_.setTexture(texture_, true);
    sf::FloatRect sprite_bounds = sprite_.getLocalBounds();
    sprite_.setOrigin(sprite_bounds.width / 2, sprite_bounds.height / 2);

    return true;
}

void BaseEntity::setPosiition(util::X abs_x, util::Y abs_y) {
    trans_.setPosition(abs_x, abs_y);
}

void BaseEntity::setHitbox(util::Right right, util::Left left, util::Top top, util::Bottom bottom) {
    hitbox_.setHitbox(right, left, top, bottom);
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
    setPosiition(util::X(j["position"]["x"].get<float>()),
                 util::Y(j["position"]["y"].get<float>()));

    auto right = util::Right(j["hitbox"]["right"].get<double>());
    auto left = util::Left(j["hitbox"]["left"].get<double>());
    auto top = util::Top(j["hitbox"]["top"].get<double>());
    auto bottom = util::Bottom(j["hitbox"]["bottom"].get<double>());
    setHitbox(right, left, top, bottom);

    loadTexture(j["sprite"].get<std::string>());
}

void BaseEntity::setTextureCoords(std::pair<util::Point, util::Point> rect) {
    sprite_.setTextureRect(sf::IntRect(rect.first.x, rect.first.y, rect.second.x, rect.second.y));
    sprite_.setOrigin(rect.second.x / 2, rect.second.y / 2);
    // Scale to 100px, keep aspect ratio
    sprite_.setScale(200.0 / rect.second.y, 200.0 / rect.second.y);
}

void BaseEntity::update() {
}

void BaseEntity::render(sf::RenderWindow& window) {
    sprite_.setPosition(trans_.getX(), trans_.getY());
    window.draw(sprite_);

    // TODO Move to debug menu
    sf::RectangleShape rectangle(sf::Vector2f(hitbox_.right_ - hitbox_.left_, hitbox_.bottom_ - hitbox_.top_));
    rectangle.setPosition(trans_.getX() + hitbox_.left_, trans_.getY() + hitbox_.top_);
    rectangle.setFillColor(sf::Color(255, 0, 0, 64));
    window.draw(rectangle);
}

