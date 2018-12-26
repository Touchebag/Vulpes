#include "player.h"
#include "world.h"

#include <iostream>
#include <tuple>

void Player::update() {
    vec2d vel;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        vel.x = -10;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        vel.x = 10;
    }

    move(vel);
}

void Player::move(vec2d velocity) {
    // Gravity
    // TODO Max limit
    velocity_.y = velocity_.y + 1;

    velocity_.x = velocity.x;

    moveAndCheckCollision();
}

void Player::moveAndCheckCollision() {
    float x = velocity_.x;
    float y = velocity_.y;
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

        // Readjust abs_hitbox to new values
        abs_hitbox.left_ += x - velocity_.x;
        abs_hitbox.right_ += x - velocity_.x;
        abs_hitbox.top_ += y - velocity_.y;
        abs_hitbox.bottom_ += y - velocity_.y;
    }

    velocity_.x = x;
    velocity_.y = y;
    trans_.move(velocity_.x, velocity_.y);
}

