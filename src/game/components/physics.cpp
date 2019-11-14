#include "physics.h"

#include "utils/log.h"

Physics::Physics(std::weak_ptr<StatefulEntity> statefulEntity, std::weak_ptr<RenderableEntity> renderableEntity, std::weak_ptr<MovableEntity> movableEntity, std::weak_ptr<AnimatedEntity> animatedEntity, std::weak_ptr<Actions> actions, std::weak_ptr<Collision> collision) :
                 statefulEntity_(statefulEntity),
                 renderableEntity_(renderableEntity),
                 movableEntity_(movableEntity),
                 animatedEntity_(animatedEntity),
                 actions_(actions),
                 collision_(collision) {
}

void Physics::update() {
    // TODO Move somwhere more logical (transform?)
    bool facing_right = true;
    if (auto rndr = renderableEntity_.lock()) {
        facing_right = rndr->facing_right_;
    }

    auto stateEnt = statefulEntity_.lock();
    auto movable = movableEntity_.lock();
    auto act = actions_.lock();

    if (stateEnt && movable && act) {
            double x = movable->getVelX();
            double y = movable->getVelY();

            if (!stateEnt->getStateProperties().movement_locked_x_) {
                if (act->getActionState(Actions::Action::MOVE_LEFT)) {
                    if (stateEnt->getStateProperties().touching_ground_) {
                        x -= constants_.ground_accel;
                    } else if (stateEnt->getStateProperties().touching_wall_ && facing_right) {
                        // Pressing away from wall should let go
                        stateEnt->incomingEvent(state::Event::LEAVING_WALL);
                    } else {
                        x -= constants_.air_accel;
                    }
                    stateEnt->incomingEvent(state::Event::MOVING);

                    // When moving left facing_right should be false even when speed is zero
                    facing_right = x > 0.0;
                } else if (act->getActionState(Actions::Action::MOVE_RIGHT)) {
                    if (stateEnt->getStateProperties().touching_ground_) {
                        x += constants_.ground_accel;
                    } else if (stateEnt->getStateProperties().touching_wall_ && !facing_right) {
                        // Pressing away from wall should let go
                        stateEnt->incomingEvent(state::Event::LEAVING_WALL);
                    } else {
                        x += constants_.air_accel;
                    }
                    stateEnt->incomingEvent(state::Event::MOVING);

                    // When moving right facing_right should be true even when speed is zero
                    facing_right = x >= 0.0;
                } else {
                    stateEnt->incomingEvent(state::Event::NO_MOVEMENT);
                }
            }

            if (stateEnt->getStateProperties().dashing_) {
                x *= constants_.dash_friction;
            } else if (stateEnt->getStateProperties().touching_ground_) {
                x *= constants_.ground_friction;
            } else {
                x *= constants_.air_friction;
            }

            if (!stateEnt->getStateProperties().movement_locked_y_) {
                // Gravity
                y += constants_.gravity;
            }

            if (stateEnt->getStateProperties().touching_wall_) {
                y *= constants_.wall_slide_friction;
            }

            if (act->getActionState(Actions::Action::DASH, true)) {
                if (stateEnt->getStateProperties().can_dash_) {
                    x = constants_.dash_speed * (facing_right ? 1.0 : -1.0);
                    y = 0.0;
                    stateEnt->incomingEvent(state::Event::DASHING);
                }
            }

            if (act->getActionState(Actions::Action::JUMP, true)) {
                if (stateEnt->getStateProperties().touching_wall_) {
                    facing_right = !facing_right;
                    int dir = facing_right ? 1.0 : -1.0;
                    x = constants_.wall_jump_horizontal_impulse * dir;
                    y = constants_.wall_jump_vertical_impulse;

                    stateEnt->incomingEvent(state::Event::JUMPING);
                } else if (stateEnt->getStateProperties().can_jump_) {
                    y = constants_.jump_impulse;
                    stateEnt->incomingEvent(state::Event::JUMPING);
                }
            }

            y = std::max(std::min(y, constants_.max_vertical_speed), constants_.min_vertical_speed);
            auto max_movement = movable->getMaximumMovement(x, y);

            if (max_movement.second < y) {
                stateEnt->incomingEvent(state::Event::TOUCHING_FLOOR);
            } else if (static_cast<int>(max_movement.first) != static_cast<int>(x)) {
                stateEnt->incomingEvent(state::Event::TOUCHING_WALL);
            } else if (max_movement.second > 0.0) {
                stateEnt->incomingEvent(state::Event::FALLING);
            }

            movable->move(max_movement.first, max_movement.second);
    }

    if (auto render = renderableEntity_.lock()) {
        render->facing_right_ = facing_right;
    }
}

void Physics::setPhysicsConstants(Constants constants) {
    constants_ = constants;
}

void Physics::loadFromJson(nlohmann::json j) {
    Constants constants;

    if (j.contains("ground_acceleration")) {
        constants.ground_accel = j["ground_acceleration"].get<double>();
    }
    if (j.contains("ground_friction")) {
        constants.ground_friction = j["ground_friction"].get<double>();
    }
    if (j.contains("air_acceleration")) {
        constants.air_accel = j["air_acceleration"].get<double>();
    }
    if (j.contains("air_friction")) {
        constants.air_friction = j["air_friction"].get<double>();
    }
    if (j.contains("gravity")) {
        constants.gravity = j["gravity"].get<double>();
    }
    if (j.contains("max_vertical_speed")) {
        constants.max_vertical_speed = j["max_vertical_speed"].get<double>();
    }
    if (j.contains("min_vertical_speed")) {
        constants.min_vertical_speed = j["min_vertical_speed"].get<double>();
    }
    if (j.contains("jump_impulse")) {
        constants.jump_impulse = j["jump_impulse"].get<double>();
    }
    if (j.contains("wall_slide_friction")) {
        constants.wall_slide_friction = j["wall_slide_friction"].get<double>();
    }
    if (j.contains("wall_jump_horizontal_impulse")) {
        constants.wall_jump_horizontal_impulse = j["wall_jump_horizontal_impulse"].get<double>();
    }
    if (j.contains("wall_jump_vertical_impulse")) {
        constants.wall_jump_vertical_impulse = j["wall_jump_vertical_impulse"].get<double>();
    }
    if (j.contains("dash_speed")) {
        constants.dash_speed = j["dash_speed"].get<double>();
    }
    if (j.contains("dash_friction")) {
        constants.dash_friction = j["dash_friction"].get<double>();
    }

    setPhysicsConstants(constants);
}

std::optional<nlohmann::json> Physics::outputToJson() {
    nlohmann::json j;
    return j;
}
