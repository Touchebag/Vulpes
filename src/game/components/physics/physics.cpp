#include "physics.h"

#include "system/system.h"

#include "utils/log.h"

#include "components/component_store.h"

#define Q(str) #str
#define QUOTE(str) Q(str)

#define loadConstant(name)\
    if (j.contains(QUOTE(name))) {\
        constants.name = j[QUOTE(name)].get<double>();\
    }


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
    auto state = getComponent<Stateful>();
    auto move = getComponent<Movement>();
    auto act = getComponent<Actions>();

    if (state && move && act) {
        auto physics_props = state->getPhysicsProperties();

        // Respond to previous frame's movement
        auto move_attr = move->getMovementAttributes();

        if (move_attr.on_ground) {
            state->incomingEvent(state_utils::Event::TOUCHING_FLOOR);
        } else if (move_attr.touching_wall) {
            state->incomingEvent(state_utils::Event::TOUCHING_WALL);
        } else {
            state->incomingEvent(state_utils::Event::AIRBORNE);
        }

        if (move_attr.falling) {
            state->incomingEvent(state_utils::Event::FALLING);
        }

        FacingDirection facing_right;

        facing_right.setDirection(move->isFacingRight());

        facing_right.lockDirection(physics_props.direction_locked_);

        // Used to set movement direction
        double x_movement_direction = 0.0;
        if (!physics_props.movement_locked_x_) {
            if (act->getActionState(Actions::Action::MOVE_LEFT)) {
                x_movement_direction = -1.0;
                state->incomingEvent(state_utils::Event::MOVING_X);

                // When moving left facing_right should be false even when speed is zero
                facing_right.setDirection(move->getVelX() > 0.0);
            } else if (act->getActionState(Actions::Action::MOVE_RIGHT)) {
                x_movement_direction = 1.0;
                state->incomingEvent(state_utils::Event::MOVING_X);

                // When moving right facing_right should be true even when speed is zero
                facing_right.setDirection(move->getVelX() >= 0.0);
            } else if (act->getActionState(Actions::Action::MOVE_FORWARD)) {
                // This is separate to give priority to explicit movement
                x_movement_direction = facing_right ? 1.0 : -1.0;
            } else {
                state->incomingEvent(state_utils::Event::NO_MOVEMENT_X);
            }
        }

        double y_movement_direction = 0.0;
        if (!physics_props.movement_locked_y_) {
            if (act->getActionState(Actions::Action::MOVE_UP)) {
                y_movement_direction = -1.0;
                state->incomingEvent(state_utils::Event::MOVING_Y);
            } else if (act->getActionState(Actions::Action::MOVE_DOWN)) {
                y_movement_direction = 1.0;
                state->incomingEvent(state_utils::Event::MOVING_Y);
            } else {
                state->incomingEvent(state_utils::Event::NO_MOVEMENT_Y);
            }
        }

        // Velocity modifications
        double x_additive = 0.0;
        double x_multiplicative = 1.0;
        double y_additive = 0.0;
        double y_multiplicative = 1.0;

        // Apply movement
        x_additive += constants_.x_acceleration * x_movement_direction;
        y_additive += constants_.y_acceleration * y_movement_direction;

        x_multiplicative *= constants_.x_friction;

        if (!physics_props.movement_locked_y_) {
            // Only apply jump multiplier if holding jump and moving upwards
            auto multiplier = (act->getActionState(Actions::Action::JUMP)
                               && move->getVelY() < 0.0) ? constants_.jump_multiplier : 1.0;
            y_additive += constants_.gravity * multiplier;
        }

        if (act->getActionState(Actions::Action::DASH, true)) {
            // If holding a direction dash in that direction
            // else dash forward
            if (act->getActionState(Actions::Action::MOVE_RIGHT)) {
                move->setVelocity(constants_.dash_speed, 0.0);
                facing_right.setDirection(true);
            } else if (act->getActionState(Actions::Action::MOVE_LEFT)) {
                move->setVelocity(-constants_.dash_speed, 0.0);
                facing_right.setDirection(false);
            } else {
                move->setVelocity(constants_.dash_speed * (facing_right ? 1.0 : -1.0), 0.0);
            }
            state->incomingEvent(state_utils::Event::DASHING);
        }

        if (act->getActionState(Actions::Action::JUMP, true)) {
            if (!std::isnan(constants_.jump_impulse_x)) {
                int dir = facing_right ? 1.0 : -1.0;
                move->setVelocity(constants_.jump_impulse_x * dir, move->getVelY());
            }
            if (!std::isnan(constants_.jump_impulse_y)) {
                move->setVelocity(move->getVelX(), constants_.jump_impulse_y);
            }
            state->incomingEvent(state_utils::Event::JUMPING);
        }

        y_multiplicative *= constants_.y_friction;

        // Fetch movement after potentially modifying via state change
        double x = (move->getVelX() + x_additive) * x_multiplicative;
        double y = (move->getVelY() + y_additive) * y_multiplicative;
        move->setVelocity(x, y);
        move->setFacingRight(facing_right);
    } else {
        if (!state) {
            LOGW("Physics: Missing state");
        }
        if (!move) {
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

void Physics::setPhysicsConstants() {
    constants_ = original_constants_;
}

PhysicsConstants Physics::getPhysicsConstants() {
    return original_constants_;
}

std::shared_ptr<Physics> Physics::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<Physics>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

PhysicsConstants Physics::loadConstantsFromJson(nlohmann::json j, PhysicsConstants constants) {
    loadConstant(x_acceleration);
    loadConstant(x_friction);

    loadConstant(y_acceleration);
    loadConstant(y_friction);
    loadConstant(gravity);
    loadConstant(jump_multiplier);

    loadConstant(jump_impulse_x);
    loadConstant(jump_impulse_y);
    loadConstant(dash_speed);

    return constants;
}

void Physics::reloadFromJson(nlohmann::json j, File /* file_instance */) {
    original_constants_ = loadConstantsFromJson(j, {});
    setPhysicsConstants(original_constants_);
}

std::optional<nlohmann::json> Physics::outputToJson() {
    nlohmann::json j;
    const PhysicsConstants default_constants;

    // Only save changed constants
    saveConstantToJson(j, "x_acceleration", original_constants_.x_acceleration, default_constants.x_acceleration);
    saveConstantToJson(j, "x_friction", original_constants_.x_friction, default_constants.x_friction);

    saveConstantToJson(j, "gravity", original_constants_.gravity, default_constants.gravity);
    saveConstantToJson(j, "jump_multiplier", original_constants_.jump_multiplier, default_constants.jump_multiplier);
    saveConstantToJson(j, "y_acceleration", original_constants_.y_acceleration, default_constants.y_acceleration);
    saveConstantToJson(j, "y_friction", original_constants_.y_friction, default_constants.y_friction);

    saveConstantToJson(j, "jump_impulse_x", original_constants_.jump_impulse_x, default_constants.jump_impulse_x);
    saveConstantToJson(j, "jump_impulse_y", original_constants_.jump_impulse_y, default_constants.jump_impulse_y);
    saveConstantToJson(j, "dash_speed", original_constants_.dash_speed, default_constants.dash_speed);

    return j;
}
