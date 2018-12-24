#include "base_entity.h"

BaseEntity::BaseEntity() {
    if (!texture_.loadFromFile("box.png")) {
        exit(1);
    }

    sprite_.setTexture(texture_);
    sf::FloatRect sprite_bounds = sprite_.getLocalBounds();
    sprite_.setOrigin(sprite_bounds.width / 2, sprite_bounds.height / 2);
}

void BaseEntity::setPosiition(float abs_x, float abs_y) {
    trans_.setPosition(abs_x, abs_y);
}

void BaseEntity::setHitbox(float left, float right, float top, float bottom) {
    hitbox_.setHitbox(left, right, top, bottom);
}

Hitbox BaseEntity::getAbsHitbox() {
    Hitbox abs_hitbox;
    abs_hitbox.setHitbox(hitbox_.right_ + trans_.getX(), hitbox_.left_ + trans_.getX(), hitbox_.top_ + trans_.getY(), hitbox_.bottom_ + trans_.getY());
    return abs_hitbox;
}

void BaseEntity::loadFromJson(nlohmann::json j) {
    setPosiition(j["position"]["x"].get<float>(), j["position"]["y"].get<float>());
    setHitbox(j["hitbox"]["right"].get<float>(), j["hitbox"]["left"].get<float>(), j["hitbox"]["top"].get<float>(), j["hitbox"]["bottom"].get<float>());
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

