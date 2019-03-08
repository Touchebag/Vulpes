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

    int x = velx_, y = vely_;

    if (!getStateProperties().movement_locked_x_) {
        if (Input::getInstance().isButtonHeld(input::button::LEFT)) {
            if (getStateProperties().touching_ground_) {
                x = -10;
            } else {
                x = std::max(x - 1, -10);
            }
            incomingEvent(state::Event::MOVING);

            // When moving left facing_right_ should be false even when speed is zero
            facing_right_ = x > 0;
        } else if (Input::getInstance().isButtonHeld(input::button::RIGHT)) {
            if (getStateProperties().touching_ground_) {
                x = 10;
            } else {
                x = std::min(x + 1, 10);
            }
            incomingEvent(state::Event::MOVING);

            // When moving right facing_right_ should be true even when speed is zero
            facing_right_ = x >= 0;
        } else {
            if (getStateProperties().touching_ground_) {
                x /= 5;
            }
            incomingEvent(state::Event::NO_MOVEMENT);
        }
    }

    if (!getStateProperties().movement_locked_y_) {
        // Gravity
        y += 1;
    }

    if (getStateProperties().touching_wall_) {
        y = std::min(y, 5);
    }

    if (Input::getInstance().isButtonPressed(input::button::DASH)) {
        if (getStateProperties().can_dash_) {
            x = 50 * (facing_right_ ? 1 : -1);
            y = 0;
            incomingEvent(state::Event::DASHING);
        }
    }

    if (Input::getInstance().isButtonPressed(input::button::JUMP)) {
        if (getStateProperties().touching_wall_) {
            facing_right_ = !facing_right_;
            int dir = facing_right_ ? 1 : -1;
            x = 10 * dir;
            y = -20;

            incomingEvent(state::Event::JUMPING);
        } else if (getStateProperties().can_jump_) {
            y = -20;
            incomingEvent(state::Event::JUMPING);
        }
    }

    move(util::X(x), util::Y(y));

    updateState();
    auto sprite_rect = getSpriteRect(getCurrentSprite());
    setTextureCoords(sprite_rect);
}

void Player::move(util::X velx, util::Y vely) {
    vely_ = std::max(std::min(static_cast<int>(vely), 20), -20);

    velx_ = velx;

    moveAndCheckCollision();
}

void Player::moveAndCheckCollision() {
    int x = velx_;
    int y = vely_;
    World& worldInst = World::getInstance();

    Hitbox abs_hitbox = getAbsHitbox();
    abs_hitbox.left_ += x;
    abs_hitbox.right_ += x;
    abs_hitbox.top_ += y;
    abs_hitbox.bottom_ += y;

    for (auto it = worldInst.getWorldObjects().begin(); it != worldInst.getWorldObjects().end(); ++it) {
        Hitbox other_hitbox = (*it)->getAbsHitbox();

        if (other_hitbox.collides(abs_hitbox)) {
            std::tuple<int, int> newMoveValues = getAbsHitbox().getMaximumMovement(util::X(x), util::Y(y), other_hitbox);
            x = std::get<0>(newMoveValues);
            y = std::get<1>(newMoveValues);

            // Readjust abs_hitbox to new values
            abs_hitbox.left_ += x - velx_;
            abs_hitbox.right_ += x - velx_;
            abs_hitbox.top_ += y - vely_;
            abs_hitbox.bottom_ += y - vely_;
        }
    }

    if (y < vely_) {
        incomingEvent(state::Event::TOUCHING_FLOOR);
    } else if (x != velx_) {
        incomingEvent(state::Event::TOUCHING_WALL);
    } else if (y > 0) {
        incomingEvent(state::Event::FALLING);
    }

    velx_ = util::X(x);
    vely_ = util::Y(y);
    trans_.move(velx_, vely_);
}

