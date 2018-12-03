#include "base_entity.h"

BaseEntity::BaseEntity() {
    if (!texture_.loadFromFile("box.png")) {
        exit(1);
    }

    sprite_.setTexture(texture_);
}

void BaseEntity::update() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        sprite_.move(-0.1, 0);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        sprite_.move(0.1, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        sprite_.move(0, -0.1);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        sprite_.move(0, 0.1);
    }
}
