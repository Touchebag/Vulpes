#include "physics.h"

#include "system/system.h"

#include "utils/log.h"

#include "components/component_store.h"

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

void saveConstantToJson(nlohmann::json& j, std::string name, double constant, double default_constant) {
    if (constant != default_constant) {
        j[name] = constant;
    }
}

} // namespace

Physics::Physics(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void Physics::update() {
    // TODO Move somwhere more logical (transform?)
    auto stateEnt = getComponent<StatefulEntity>();
    auto movable = getComponent<MovableEntity>();
    auto act = getComponent<Actions>();

    if (stateEnt && movable && act) {
        double x = movable->getVelX();
        double y = movable->getVelY();

        auto physics_props = stateEnt->getPhysicsProperties();

        // Respond to previous frame's movement
        auto move_attr = movable->getMovementAttributes();

        if (move_attr.on_ground) {
            if (physics_props.air_diving_) {
                // If we touch ground while diving we landed this frame
                System::getCamera()->addTrauma(0.4f);
            }
            stateEnt->incomingEvent(state_utils::Event::TOUCHING_FLOOR);
        } else if (move_attr.touching_wall) {
            stateEnt->incomingEvent(state_utils::Event::TOUCHING_WALL);
        } else {
            stateEnt->incomingEvent(state_utils::Event::AIRBORNE);
        }

        if (move_attr.falling) {
            stateEnt->incomingEvent(state_utils::Event::FALLING);
        }

        FacingDirection facing_right;

        facing_right.setDirection(movable->isFacingRight());

        facing_right.lockDirection(physics_props.direction_locked_);

        if (physics_props.touching_ground_) {
            variables_.resetJumps();
            variables_.resetDashes();
        }

        if (!physics_props.movement_locked_x_) {
            if (act->getActionState(Actions::Action::MOVE_LEFT)) {
                if (physics_props.touching_ground_) {
                    x -= constants_.ground_acceleration;
                } else {
                    x -= constants_.air_acceleration;
                }
                stateEnt->incomingEvent(state_utils::Event::MOVING);

                // When moving left facing_right should be false even when speed is zero
                facing_right.setDirection(x > 0.0);
            } else if (act->getActionState(Actions::Action::MOVE_RIGHT)) {
                if (physics_props.touching_ground_) {
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

        if (physics_props.dashing_) {
            x *= constants_.dash_friction;
        } else if (physics_props.touching_ground_) {
            x *= constants_.ground_friction;
        } else {
            x *= constants_.air_friction;
        }

        auto fall_mult = constants_.fall_multiplier;
        if (physics_props.air_diving_) {
            fall_mult *= constants_.air_dive_multiplier;
        }

        if (!physics_props.movement_locked_y_) {
            // Gravity
            if (y > 0.0 && !physics_props.touching_wall_) {
                y += constants_.gravity * fall_mult;
            } else if (y < 0.0
                       && !(act->getActionState(Actions::Action::JUMP))
                       && !physics_props.touching_wall_) {
                y += constants_.gravity * constants_.low_jump_multiplier;
            } else {
                y += constants_.gravity;
            }
        }

        // Only slide when falling
        if (y > 0.0 && physics_props.touching_wall_) {
            y *= constants_.wall_slide_friction;
        }

        if (act->getActionState(Actions::Action::DASH, true)) {
            if (physics_props.can_dash_ && variables_.popDashes()) {
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
            if (physics_props.can_jump_ && physics_props.touching_ground_ && !physics_props.touching_wall_) {
                y = constants_.jump_impulse;
                stateEnt->incomingEvent(state_utils::Event::JUMPING);
            }
        }

        if (act->getActionState(Actions::Action::DOUBLE_JUMP, true)) {
            if (physics_props.can_jump_ &&
                !physics_props.touching_ground_ &&
                !physics_props.touching_wall_ &&
                variables_.popJumps()) {
                    y = constants_.jump_impulse;
                    stateEnt->incomingEvent(state_utils::Event::JUMPING);
            }
        }

        if (act->getActionState(Actions::Action::WALL_JUMP, true)) {
            if (physics_props.touching_wall_) {
                stateEnt->incomingEvent(state_utils::Event::JUMPING);
                facing_right.lockDirection(physics_props.direction_locked_);

                facing_right.setDirection(!facing_right);
                int dir = facing_right ? -1.0 : 1.0;
                x = constants_.wall_jump_horizontal_impulse * dir;
                y = constants_.wall_jump_vertical_impulse;

                // Return aerial jumps on wall jump
                variables_.resetJumps();
                variables_.resetDashes();
            }
        }

        // No need to check whether we are allowed to attack since there is no special
        // physics handling. All actions are handled by states.
        if (act->getActionState(Actions::Action::ATTACK)) {
            stateEnt->incomingEvent(state_utils::Event::ATTACKING);
        }

        if (act->getActionState(Actions::Action::AIR_DIVE, true) && physics_props.can_air_dive_) {
            stateEnt->incomingEvent(state_utils::Event::AIR_DIVING);
            x = 0.0;
            y = constants_.air_dive_impulse;
        }

        if (physics_props.air_diving_) {
            // Air dive only falls
            y = std::min(y, constants_.max_air_dive_speed);
        } else {
            y = std::max(std::min(y, constants_.max_vertical_speed), constants_.min_vertical_speed);
        }

        if (act->getActionState(Actions::Action::AIR_DIVE_BOUNCE, true)) {
            y = -30;
            stateEnt->incomingEvent(state_utils::Event::DIVE_BOUNCE);
            variables_.resetJumps();
            variables_.resetDashes();
        }

        movable->setFacingRight(facing_right);
        movable->setVelocity(x, y);
    } else {
        if (!stateEnt) {
            LOGW("Physics: Missing state");
        }
        if (!movable) {
            LOGW("Physics: Missing movable");
        }
        if (!act) {
            LOGW("Physics: Missing actions");
        }
    }
}

void Physics::setPhysicsConstants(PhysicsConstants constants) {
    constants_ = constants;
}

void Physics::setPhysicsVariables() {
    variables_.setMaxJumps(1);
    variables_.setMaxDashes(1);
}

std::shared_ptr<Physics> Physics::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<Physics>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void Physics::reloadFromJson(nlohmann::json j, File /* file_instance */) {
    PhysicsConstants constants;

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
    if (j.contains("max_air_dive_speed")) {
        constants.max_air_dive_speed = j["max_air_dive_speed"].get<double>();
    }
    if (j.contains("air_dive_multiplier")) {
        constants.air_dive_multiplier = j["air_dive_multiplier"].get<double>();
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
    if (j.contains("air_dive_impulse")) {
        constants.air_dive_impulse = j["air_dive_impulse"].get<double>();
    }

    setPhysicsConstants(constants);
    // TODO Read from json
    setPhysicsVariables();
}

std::optional<nlohmann::json> Physics::outputToJson() {
    nlohmann::json j;
    const PhysicsConstants default_constants;

    // Only save changed constants
    saveConstantToJson(j, "ground_acceleration", constants_.ground_acceleration, default_constants.ground_acceleration);
    saveConstantToJson(j, "ground_friction", constants_.ground_friction, default_constants.ground_friction);

    saveConstantToJson(j, "air_acceleration", constants_.air_acceleration, default_constants.air_acceleration);
    saveConstantToJson(j, "air_friction", constants_.air_friction, default_constants.air_friction);
    saveConstantToJson(j, "gravity", constants_.gravity, default_constants.gravity);
    saveConstantToJson(j, "fall_multiplier", constants_.fall_multiplier, default_constants.fall_multiplier);
    saveConstantToJson(j, "low_jump_multiplier", constants_.low_jump_multiplier, default_constants.low_jump_multiplier);
    saveConstantToJson(j, "min_vertical_speed", constants_.min_vertical_speed, default_constants.min_vertical_speed);
    saveConstantToJson(j, "max_vertical_speed", constants_.max_vertical_speed, default_constants.max_vertical_speed);
    saveConstantToJson(j, "max_air_dive_speed", constants_.max_air_dive_speed, default_constants.max_air_dive_speed);
    saveConstantToJson(j, "air_dive_multiplier", constants_.air_dive_multiplier, default_constants.air_dive_multiplier);

    saveConstantToJson(j, "jump_impulse", constants_.jump_impulse, default_constants.jump_impulse);
    saveConstantToJson(j, "wall_slide_friction", constants_.wall_slide_friction, default_constants.wall_slide_friction);
    saveConstantToJson(j, "wall_jump_horizontal_impulse", constants_.wall_jump_horizontal_impulse, default_constants.wall_jump_horizontal_impulse);
    saveConstantToJson(j, "wall_jump_vertical_impulse", constants_.wall_jump_vertical_impulse, default_constants.wall_jump_vertical_impulse);
    saveConstantToJson(j, "dash_speed", constants_.dash_speed, default_constants.dash_speed);
    saveConstantToJson(j, "dash_friction", constants_.dash_friction, default_constants.dash_friction);
    saveConstantToJson(j, "air_dive_impulse", constants_.air_dive_impulse, default_constants.air_dive_impulse);
    return j;
}
