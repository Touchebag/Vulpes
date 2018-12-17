#include "player.h"
#include "world.h"

#include <iostream>

void Player::update() {
    float move_x = 0.0;
    float move_y = 0.0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        move_x = -0.1;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        move_x = 0.1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        move_y = -0.1;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        move_y = 0.1;
    }

    moveAndCheckCollision(move_x, move_y);
}

void Player::moveAndCheckCollision(float x, float y) {
    World& worldInst = World::getInstance();
    Hitbox abs_hitbox = getAbsHitbox();
    abs_hitbox.left_ += x;
    abs_hitbox.right_ += x;
    abs_hitbox.top_ += y;
    abs_hitbox.bottom_ += y;

    for (auto it = worldInst.getWorldObjects().begin(); it != worldInst.getWorldObjects().end(); ++it) {
        if ((*it)->getAbsHitbox().collides(abs_hitbox)) {
            return;
        }
    }

    trans_.move(x, y);
}
