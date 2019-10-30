#include "physics.h"

#include "log.h"

Physics::Physics(std::weak_ptr<StatefulEntity> statefulEntity, std::weak_ptr<RenderableEntity> renderableEntity, std::weak_ptr<MovableEntity> movableEntity, std::weak_ptr<AnimatedEntity> animatedEntity, std::weak_ptr<Actions> actions, std::weak_ptr<Collision> collision) :
                 statefulEntity_(statefulEntity),
                 renderableEntity_(renderableEntity),
                 movableEntity_(movableEntity),
                 animatedEntity_(animatedEntity),
                 actions_(actions),
                 collision_(collision) {
}

void Physics::update() {
    bool facing_right = true;
    if (auto rndr = renderableEntity_.lock()) {
        facing_right = rndr->facing_right_;
    }

    auto stateEnt = statefulEntity_.lock();
    auto movable = movableEntity_.lock();
    auto act = actions_.lock();

    if (stateEnt) {
        if (movable && act) {
            double x = movable->getVelX();
            double y = movable->getVelY();

            if (!stateEnt->getStateProperties().movement_locked_x_) {
                if (act->getActionState(Actions::Action::MOVE_LEFT)) {
                    if (stateEnt->getStateProperties().touching_ground_) {
                        x = -10.0;
                    } else {
                        x = std::max(x - 1.0, -10.0);
                    }
                    stateEnt->incomingEvent(state::Event::MOVING);

                    // When moving left facing_right should be false even when speed is zero
                    facing_right = x > 0.0;
                } else if (act->getActionState(Actions::Action::MOVE_RIGHT)) {
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

            if (act->getActionState(Actions::Action::DASH, true)) {
                if (stateEnt->getStateProperties().can_dash_) {
                    x = 50.0 * (facing_right ? 1.0 : -1.0);
                    y = 0.0;
                    stateEnt->incomingEvent(state::Event::DASHING);
                }
            }

            if (act->getActionState(Actions::Action::JUMP, true)) {
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
            auto max_movement = movable->getMaximumMovement(x, y);

            if (static_cast<int>(max_movement.second) < y) {
                stateEnt->incomingEvent(state::Event::TOUCHING_FLOOR);
            } else if (static_cast<int>(max_movement.first) != static_cast<int>(x)) {
                stateEnt->incomingEvent(state::Event::TOUCHING_WALL);
            } else if (static_cast<int>(max_movement.second) > 0.0) {
                stateEnt->incomingEvent(state::Event::FALLING);
            }

            movable->move(max_movement.first, max_movement.second);
        }
    }

    if (auto render = renderableEntity_.lock()) {
        render->facing_right_ = facing_right;
    }
}

void Physics::loadFromJson(nlohmann::json j) {
    // TODO Load constants
}

std::optional<nlohmann::json> Physics::outputToJson() {
    nlohmann::json j;
    return j;
}
