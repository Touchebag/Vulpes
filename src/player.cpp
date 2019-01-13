#include "player.h"
#include "world.h"
#include "input_event.h"

#include <tuple>
#include <algorithm>

void Player::update() {
    // Check before decreasing
    // If previous frame was the last one (i.e. ticked down to 0) then trigger event before this frame
    if (frame_counter_-- == 0) {
        incomingEvent(state::Event::FRAME_TIMEOUT);
    }

    double x = 0.0, y = 0.0;

    if (Input::getInstance().isButtonHeld(input::button::LEFT)) {
        x = -10.0;
    } else if (Input::getInstance().isButtonHeld(input::button::RIGHT)) {
        x = 10.0;
    }

    move(util::X(x), util::Y(y));

    if (Input::getInstance().isButtonPressed(input::button::JUMP)) {
        if (getProperty(state::Property::TOUCHING_GROUND) &&
            !getProperty(state::Property::MOVEMENT_LOCKED)) {
            velY_ = util::Y(-20.0);
        } else if (getProperty(state::Property::TOUCHING_RIGHT_WALL)) {
            velY_ = util::Y(-20.0);
            velX_ = util::X(-20.0);
        } else if (getProperty(state::Property::TOUCHING_LEFT_WALL)) {
            velY_ = util::Y(-20.0);
            velX_ = util::X(20.0);
        }

        incomingEvent(state::Event::JUMP);
    }
}

void Player::move(util::X velX, util::Y velY) {
    // Gravity
    velY_ = std::min(velY_ + 1.0, 20.0);

    if (!getProperty(state::Property::MOVEMENT_LOCKED)) {
        velX_ = velX;
    }

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

    if (x < velX_) {
        incomingEvent(state::Event::TOUCHING_WALL_RIGHT);
    } else if (x > velX_) {
        incomingEvent(state::Event::TOUCHING_WALL_LEFT);
    }

    if (y < velY_) {
        incomingEvent(state::Event::TOUCHING_FLOOR);
    }

    velX_ = util::X(x);
    velY_ = util::Y(y);
    trans_.move(velX_, velY_);
}

