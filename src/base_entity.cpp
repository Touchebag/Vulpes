#include "base_entity.h"

BaseEntity::BaseEntity() {
    if (!texture_.loadFromFile("box.png")) {
        exit(1);
    }

    sprite_.setTexture(texture_);
}

void BaseEntity::setPosiition(float abs_x, float abs_y) {
    sprite_.setPosition(abs_x, abs_y);
}

void BaseEntity::update() {
}

void BaseEntity::render(sf::RenderWindow& window) {
    window.draw(sprite_);
}

void BaseEntity::move(float rel_x,float rel_y) {
    sprite_.move(rel_x, rel_y);
}
