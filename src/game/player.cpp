#include "player.h"
#include "world.h"
#include "input_event.h"
#include "log.h"

#include <tuple>
#include <algorithm>

void Player::update() {
    // Check before decreasing
    // If previous frame was the last one (i.e. ticked down to 0) then trigger event before this frame
    if (frame_counter_-- == 0) {
        incomingEvent(state::Event::FRAME_TIMEOUT);
    }

    double x = velX_, y = velY_;

    if (!getStateProperties().movement_locked) {
        if (Input::getInstance().isButtonHeld(input::button::LEFT)) {
            x = -10.0;
            incomingEvent(state::Event::MOVING);
            facing_right_ = false;
        } else if (Input::getInstance().isButtonHeld(input::button::RIGHT)) {
            x = 10.0;
            incomingEvent(state::Event::MOVING);
            facing_right_ = true;
        } else {
            x = 0.0;
            incomingEvent(state::Event::NO_MOVEMENT);
        }
    }

    // Gravity
    y += 1.0;

    if (getStateProperties().touching_wall) {
        y = std::min(y, 5.0);
    }

    if (Input::getInstance().isButtonPressed(input::button::JUMP)) {
        if (getStateProperties().touching_wall) {
            facing_right_ = !facing_right_;
            float dir = facing_right_ ? 1.0 : -1.0;
            x = 10.0 * dir;
            y = -20.0;

            incomingEvent(state::Event::JUMPING);
        } else if (getStateProperties().can_jump) {
            y = -20.0;
            incomingEvent(state::Event::JUMPING);
        }
    }

    move(util::X(x), util::Y(y));

    updateState();
    auto sprite_rect = get_sprite_rect(getCurrentSprite());
    setTextureCoords(sprite_rect);
}

void Player::move(util::X velX, util::Y velY) {
    velY_ = std::max(std::min((double)velY, 20.0), -20.0);

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

            // Readjust abs_hitbox to new values
            abs_hitbox.left_ += x - velX_;
            abs_hitbox.right_ += x - velX_;
            abs_hitbox.top_ += y - velY_;
            abs_hitbox.bottom_ += y - velY_;
        }
    }

    if (y < velY_) {
        incomingEvent(state::Event::TOUCHING_FLOOR);
    } else if (x != velX_) {
        incomingEvent(state::Event::TOUCHING_WALL);
    } else if (y > 0.0) {
        incomingEvent(state::Event::FALLING);
    }

    velX_ = util::X(x);
    velY_ = util::Y(y);
    trans_.move(velX_, velY_);
}

