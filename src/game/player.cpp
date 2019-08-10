#include "player.h"
#include "input_event.h"
#include "log.h"

#include <tuple>
#include <algorithm>

void Player::update() {
    // Check before decreasing
    // If previous frame was the last one (i.e. ticked down to 0) then trigger event before this frame
    if (frame_counter_-- == 0) {
        incomingEvent(state::Event::FRAME_TIMEOUT, animatedEntity_);
    }

    bool facing_right = renderableEntity_ ? renderableEntity_->facing_right_ : true;

    if (movableEntity_) {
        double x = movableEntity_->getVelX();
        double y = movableEntity_->getVelY();

        if (!getStateProperties().movement_locked_x_) {
            if (Input::getInstance().isButtonHeld(input::button::LEFT)) {
                if (getStateProperties().touching_ground_) {
                    x = -10.0;
                } else {
                    x = std::max(x - 1.0, -10.0);
                }
                incomingEvent(state::Event::MOVING, animatedEntity_);

                // When moving left facing_right should be false even when speed is zero
                facing_right = x > 0.0;
            } else if (Input::getInstance().isButtonHeld(input::button::RIGHT)) {
                if (getStateProperties().touching_ground_) {
                    x = 10.0;
                } else {
                    x = std::min(x + 1.0, 10.0);
                }
                incomingEvent(state::Event::MOVING, animatedEntity_);

                // When moving right facing_right should be true even when speed is zero
                facing_right = x >= 0.0;
            } else {
                if (getStateProperties().touching_ground_) {
                    x /= 5.0;
                }
                incomingEvent(state::Event::NO_MOVEMENT, animatedEntity_);
            }
        }

        if (!getStateProperties().movement_locked_y_) {
            // Gravity
            y += 1.0;
        }

        if (getStateProperties().touching_wall_) {
            y = std::min(y, 5.0);
        }

        if (Input::getInstance().isButtonPressed(input::button::DASH)) {
            if (getStateProperties().can_dash_) {
                x = 50.0 * (facing_right ? 1.0 : -1.0);
                y = 0.0;
                incomingEvent(state::Event::DASHING, animatedEntity_);
            }
        }

        if (Input::getInstance().isButtonPressed(input::button::JUMP)) {
            if (getStateProperties().touching_wall_) {
                facing_right = !facing_right;
                int dir = facing_right ? 1.0 : -1.0;
                x = 10.0 * dir;
                y = -20.0;

                incomingEvent(state::Event::JUMPING, animatedEntity_);
            } else if (getStateProperties().can_jump_) {
                y = -20.0;
                incomingEvent(state::Event::JUMPING, animatedEntity_);
            }
        }

        y = std::max(std::min(y, 20.0), -20.0);
        auto max_movement = movableEntity_->getMaximumMovement(x, y, getAbsHitbox());

        if (static_cast<int>(max_movement.second) < y) {
            incomingEvent(state::Event::TOUCHING_FLOOR, animatedEntity_);
        } else if (static_cast<int>(max_movement.first) != static_cast<int>(x)) {
            incomingEvent(state::Event::TOUCHING_WALL, animatedEntity_);
        } else if (static_cast<int>(max_movement.second) > 0.0) {
            incomingEvent(state::Event::FALLING, animatedEntity_);
        }

        movableEntity_->move(max_movement.first, max_movement.second);
    }

    if (renderableEntity_) {
        renderableEntity_->facing_right_ = facing_right;
    }

    if (animatedEntity_) {
        animatedEntity_->update();
    }
}

void Player::loadFromJson(nlohmann::json j) {
    BaseEntity::loadFromJson(j);

    // TODO Move to base constructor
    movableEntity_ = std::make_shared<MovableEntity>(trans_, hitbox_);

    incomingEvent(state::Event::START, animatedEntity_);
}
