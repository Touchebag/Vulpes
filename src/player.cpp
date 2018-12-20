#include "player.h"
#include "world.h"

#include <iostream>
#include <tuple>

void Player::update() {
    float move_x = 0.0;
    float move_y = 0.0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        move_x = -10;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        move_x = 10;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        move_y = -10;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        move_y = 10;
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
        Hitbox other_hitbox = (*it)->getAbsHitbox();

        if (other_hitbox.collides(abs_hitbox)) {
            std::tuple<float, float> newMoveValues = getAbsHitbox().getMaximumMovement(x, y, other_hitbox);
            x = std::get<0>(newMoveValues);
            y = std::get<1>(newMoveValues);
        }

        // TODO Apply new move values to abs_hitbox for multiple hitboxes
    }

    trans_.move(x, y);
}

