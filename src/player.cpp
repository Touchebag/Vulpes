#include "player.h"

void Player::update() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        move(-0.1, 0);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        move(0.1, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        move(0, -0.1);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        move(0, 0.1);
    }
}
