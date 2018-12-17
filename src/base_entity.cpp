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

void BaseEntity::update() {
}

void BaseEntity::render(sf::RenderWindow& window) {
    sprite_.setPosition(trans_.getX(), trans_.getY());
    window.draw(sprite_);
}

