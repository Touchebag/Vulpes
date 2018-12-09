#include "player.h"

void Player::update() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        trans_.move(-0.1, 0);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        trans_.move(0.1, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        trans_.move(0, -0.1);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        trans_.move(0, 0.1);
    }
}
