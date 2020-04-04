#include "physics.h"

#include "utils/log.h"

namespace {
    class FacingDirection {
      public:
        void lockDirection(bool direction_locked) {
            direction_locked_ = direction_locked;
        }

        operator bool() const {
            return facing_right_;
        }

        void setDirection(bool facing_right) {
            if (!direction_locked_) {
                facing_right_ = facing_right;
            }
        }

        FacingDirection& operator= (bool) = delete;

      private:
        bool facing_right_ = true;
        bool direction_locked_ = false;
    };
} // namespace

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
    auto stateEnt = statefulEntity_.lock();
    auto movable = movableEntity_.lock();
    auto act = actions_.lock();

    // TODO Store in some separate component
    int jumps_left = 0;

    if (stateEnt && movable && act) {
        double x = movable->getVelX();
        double y = movable->getVelY();

        auto state_props = stateEnt->getStateProperties();

        FacingDirection facing_right;
        if (auto rndr = renderableEntity_.lock()) {
            facing_right.setDirection(rndr->facing_right_);
        }
        facing_right.lockDirection(state_props.direction_locked_);

        if (state_props.touching_ground_ || state_props.touching_wall_) {
            jumps_left = 1;
        }

        if (!state_props.movement_locked_x_) {
            if (act->getActionState(Actions::Action::MOVE_LEFT)) {
                if (state_props.touching_ground_) {
                    x -= constants_.ground_acceleration;
                } else {
                    x -= constants_.air_acceleration;
                }
                stateEnt->incomingEvent(state_utils::Event::MOVING);

                // When moving left facing_right should be false even when speed is zero
                facing_right.setDirection(x > 0.0);
            } else if (act->getActionState(Actions::Action::MOVE_RIGHT)) {
                if (state_props.touching_ground_) {
                    x += constants_.ground_acceleration;
                } else {
                    x += constants_.air_acceleration;
                }
                stateEnt->incomingEvent(state_utils::Event::MOVING);

                // When moving right facing_right should be true even when speed is zero
                facing_right.setDirection(x >= 0.0);
            } else {
                stateEnt->incomingEvent(state_utils::Event::NO_MOVEMENT);
            }
        }

        if (state_props.dashing_) {
            x *= constants_.dash_friction;
        } else if (state_props.touching_ground_) {
            x *= constants_.ground_friction;
        } else {
            x *= constants_.air_friction;
        }

        if (!state_props.movement_locked_y_) {
            // Gravity
            if (y > 0.0 && !state_props.touching_wall_) {
                y += constants_.gravity * constants_.fall_multiplier;
            } else if (y < 0.0
                       && !(act->getActionState(Actions::Action::JUMP))
                       && !state_props.touching_wall_) {
                y += constants_.gravity * constants_.low_jump_multiplier;
            } else {
                y += constants_.gravity;
            }
        }

        if (state_props.touching_wall_) {
            y *= constants_.wall_slide_friction;
        }

        if (act->getActionState(Actions::Action::DASH, true)) {
            if (state_props.can_dash_) {
                // If holding a direction dash in that direction
                // else dash forward
                if (act->getActionState(Actions::Action::MOVE_RIGHT)) {
                    x = constants_.dash_speed;
                    facing_right.setDirection(true);
                } else if (act->getActionState(Actions::Action::MOVE_LEFT)) {
                    x = -constants_.dash_speed;
                    facing_right.setDirection(false);
                } else {
                    x = constants_.dash_speed * (facing_right ? 1.0 : -1.0);
                }
                y = 0.0;
                stateEnt->incomingEvent(state_utils::Event::DASHING);
            }
        }

        if (act->getActionState(Actions::Action::JUMP, true)) {
            if (state_props.touching_wall_) {
                stateEnt->incomingEvent(state_utils::Event::JUMPING);
                facing_right.lockDirection(state_props.direction_locked_);

                facing_right.setDirection(!facing_right);
                int dir = facing_right ? -1.0 : 1.0;
                x = constants_.wall_jump_horizontal_impulse * dir;
                y = constants_.wall_jump_vertical_impulse;
            } else if (state_props.can_jump_ && jumps_left > 0) {
                y = constants_.jump_impulse;
                jumps_left--;
                stateEnt->incomingEvent(state_utils::Event::JUMPING);
            }
        }

        y = std::max(std::min(y, constants_.max_vertical_speed), constants_.min_vertical_speed);
        auto max_movement = movable->getMaximumMovement(x, y);

        if (max_movement.second < y) {
            stateEnt->incomingEvent(state_utils::Event::TOUCHING_FLOOR);
        } else if (max_movement.first != x) {
            stateEnt->incomingEvent(state_utils::Event::TOUCHING_WALL);
        } else if (max_movement.second > 0.0) {
            stateEnt->incomingEvent(state_utils::Event::FALLING);
        }

        movable->move(max_movement.first, max_movement.second);

        if (auto render = renderableEntity_.lock()) {
            render->facing_right_ = facing_right;
        }
    }
}

void Physics::setPhysicsConstants(Constants constants) {
    constants_ = constants;
}

void Physics::reloadFromJson(nlohmann::json j) {
    Constants constants;

    if (j.contains("ground_acceleration")) {
        constants.ground_acceleration = j["ground_acceleration"].get<double>();
    }
    if (j.contains("ground_friction")) {
        constants.ground_friction = j["ground_friction"].get<double>();
    }
    if (j.contains("air_acceleration")) {
        constants.air_acceleration = j["air_acceleration"].get<double>();
    }
    if (j.contains("air_friction")) {
        constants.air_friction = j["air_friction"].get<double>();
    }
    if (j.contains("gravity")) {
        constants.gravity = j["gravity"].get<double>();
    }
    if (j.contains("fall_multiplier")) {
        constants.fall_multiplier = j["fall_multiplier"].get<double>();
    }
    if (j.contains("low_jump_multiplier")) {
        constants.low_jump_multiplier = j["low_jump_multiplier"].get<double>();
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
