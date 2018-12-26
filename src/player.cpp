#include "player.h"
#include "world.h"

#include <iostream>
#include <tuple>

void Player::update() {
    double x = 0.0, y = 0.0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        x = -10.0;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        x = 10.0;
    }

    move(util::X(x), util::Y(y));
}

void Player::move(util::X velX, util::Y velY) {
    // Gravity
    // TODO Max limit
    velY_ += 1.0;

    velX_ = velX;

    moveAndCheckCollision();
}

void Player::moveAndCheckCollision() {
    double x = velX_;
    double y = velY_;
    World& worldInst = World::getInstance();
    Hitbox abs_hitbox = getAbsHitbox();
    abs_hitbox.left_ += x;
    abs_hitbox.right_ += x;
    abs_hitbox.top_ += y;
    abs_hitbox.bottom_ += y;

    for (auto it = worldInst.getWorldObjects().begin(); it != worldInst.getWorldObjects().end(); ++it) {
        Hitbox other_hitbox = (*it)->getAbsHitbox();

        if (other_hitbox.collides(abs_hitbox)) {
            std::tuple<float, float> newMoveValues = getAbsHitbox().getMaximumMovement(util::X(x), util::Y(y), other_hitbox);
            x = std::get<0>(newMoveValues);
            y = std::get<1>(newMoveValues);
        }

        // Readjust abs_hitbox to new values
        abs_hitbox.left_ += x - velX_;
        abs_hitbox.right_ += x - velX_;
        abs_hitbox.top_ += y - velY_;
        abs_hitbox.bottom_ += y - velY_;
    }

    velX_ = util::X(x);
    velY_ = util::Y(y);
    trans_.move(velX_, velY_);
}

