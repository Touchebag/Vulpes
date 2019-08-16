#include "player.h"
#include "input_event.h"
#include "log.h"

#include <tuple>
#include <algorithm>

void Player::update() {
    // TODO Should be part of movable?
    bool facing_right = renderableEntity_ ? renderableEntity_->facing_right_ : true;

    if (auto stateEnt = statefulEntity_) {
        // Check before decreasing
        // If previous frame was the last one (i.e. ticked down to 0) then trigger event before this frame
        stateEnt->update();

        if (movableEntity_ && actions_) {
            double x = movableEntity_->getVelX();
            double y = movableEntity_->getVelY();

            if (!stateEnt->getStateProperties().movement_locked_x_) {
                if (actions_->getActionState(Actions::Action::MOVE_LEFT)) {
                    if (stateEnt->getStateProperties().touching_ground_) {
                        x = -10.0;
                    } else {
                        x = std::max(x - 1.0, -10.0);
                    }
                    stateEnt->incomingEvent(state::Event::MOVING);

                    // When moving left facing_right should be false even when speed is zero
                    facing_right = x > 0.0;
                } else if (actions_->getActionState(Actions::Action::MOVE_RIGHT)) {
                    if (stateEnt->getStateProperties().touching_ground_) {
                        x = 10.0;
                    } else {
                        x = std::min(x + 1.0, 10.0);
                    }
                    stateEnt->incomingEvent(state::Event::MOVING);

                    // When moving right facing_right should be true even when speed is zero
                    facing_right = x >= 0.0;
                } else {
                    if (stateEnt->getStateProperties().touching_ground_) {
                        x /= 5.0;
                    }
                    stateEnt->incomingEvent(state::Event::NO_MOVEMENT);
                }
            }

            if (!stateEnt->getStateProperties().movement_locked_y_) {
                // Gravity
                y += 1.0;
            }

            if (stateEnt->getStateProperties().touching_wall_) {
                y = std::min(y, 5.0);
            }

            if (actions_->getActionState(Actions::Action::DASH, true)) {
                if (stateEnt->getStateProperties().can_dash_) {
                    x = 50.0 * (facing_right ? 1.0 : -1.0);
                    y = 0.0;
                    stateEnt->incomingEvent(state::Event::DASHING);
                }
            }

            if (actions_->getActionState(Actions::Action::JUMP, true)) {
                if (stateEnt->getStateProperties().touching_wall_) {
                    facing_right = !facing_right;
                    int dir = facing_right ? 1.0 : -1.0;
                    x = 10.0 * dir;
                    y = -20.0;

                    stateEnt->incomingEvent(state::Event::JUMPING);
                } else if (stateEnt->getStateProperties().can_jump_) {
                    y = -20.0;
                    stateEnt->incomingEvent(state::Event::JUMPING);
                }
            }

            y = std::max(std::min(y, 20.0), -20.0);
            auto max_movement = movableEntity_->getMaximumMovement(x, y, getAbsHitbox());

            if (static_cast<int>(max_movement.second) < y) {
                stateEnt->incomingEvent(state::Event::TOUCHING_FLOOR);
            } else if (static_cast<int>(max_movement.first) != static_cast<int>(x)) {
                stateEnt->incomingEvent(state::Event::TOUCHING_WALL);
            } else if (static_cast<int>(max_movement.second) > 0.0) {
                stateEnt->incomingEvent(state::Event::FALLING);
            }

            movableEntity_->move(max_movement.first, max_movement.second);
        }
    }

    if (renderableEntity_) {
        renderableEntity_->facing_right_ = facing_right;
    }

    if (animatedEntity_) {
        animatedEntity_->update();
    }

    if (actions_) {
        actions_->update();
    }
}

void Player::loadFromJson(nlohmann::json j) {
    BaseEntity::loadFromJson(j);

    if (actions_) {
        Input::getInstance().setActionsInstance(actions_);
    }

    // TODO Move to somehwere relevant
    if (auto stateEnt = statefulEntity_) {
        stateEnt->incomingEvent(state::Event::START);
    }
}
