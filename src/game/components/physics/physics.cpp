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

void setShiftedLedgeClimbPosition(std::shared_ptr<Transform> trans, std::shared_ptr<Collision> coll) {
    auto world_colls = System::IWorldRead::getCollideables(Collideable::CollisionType::STATIC);

    if (!trans || !coll) {
        return;
    }

    for (auto it = world_colls.begin(); it != world_colls.end(); ++it) {
        if (coll->collides(*it)) {
            // If this entrance would put the player inside an object
            // move player to spawn on top of object
            auto other_coll = it->lock();
            auto other_hbox = other_coll->getHitbox();
            auto other_trans = other_coll->getTransform().lock();

            auto new_y_pos = other_trans->getY() - (other_hbox->height_ / 2) -
                coll->getCollideable()->getHitbox()->height_ / 2;

            trans->setPosition(trans->getX(), new_y_pos);
        }
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
        double x = move->getVelX();
        double y = move->getVelY();

        auto physics_props = state->getPhysicsProperties();

        // Respond to previous frame's movement
        auto move_attr = move->getMovementAttributes();

        if (move_attr.on_ground) {
            if (physics_props.air_diving_) {
                // If we touch ground while diving we landed this frame
                System::getCamera()->addTrauma(0.4f);
            }
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

        // Ledge climb will override movement this frame
        if (auto coll = getComponent<Collision>()) {
            if (coll->isSensorTriggered("ledge_climb_bottom") && !coll->isSensorTriggered("ledge_climb_top")) {
                move->setVelocity(0.0, 0.0);
                // TODO Use width of sensor instead of hard coded value
                auto max_move = move->getMaximumMovement(31.0 * (facing_right ? 1.0 : -1.0), 0.0);

                // No collision would happen without transform, no validation needed
                auto trans = getComponent<Transform>();

                // Force a position inside wall
                auto coll_width = coll->getCollideable()->getHitbox()->width_;
                trans->setPosition(trans->getX() + static_cast<int>(max_move.first) + (facing_right ? coll_width : -coll_width),
                                   trans->getY());

                // Force move to on top of current collision
                setShiftedLedgeClimbPosition(trans, coll);

                state->incomingEvent(state_utils::Event::LEDGE_CLIMB);

                return;
            }
        }

        facing_right.lockDirection(physics_props.direction_locked_);

        if (physics_props.touching_ground_) {
            variables_.resetJumps();
            variables_.resetDashes();
        }

        // Used to set movement direction
        double x_movement_direction = 0.0;
        if (!physics_props.movement_locked_x_) {
            if (act->getActionState(Actions::Action::MOVE_LEFT)) {
                x_movement_direction = -1.0;
                state->incomingEvent(state_utils::Event::MOVING_X);

                // When moving left facing_right should be false even when speed is zero
                facing_right.setDirection(x > 0.0);
            } else if (act->getActionState(Actions::Action::MOVE_RIGHT)) {
                x_movement_direction = 1.0;
                state->incomingEvent(state_utils::Event::MOVING_X);

                // When moving right facing_right should be true even when speed is zero
                facing_right.setDirection(x >= 0.0);
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

        // Apply movement
        x += constants_.x_acceleration * x_movement_direction;
        y += constants_.y_acceleration * y_movement_direction;

        if (physics_props.dashing_) {
            x *= constants_.dash_friction;
        } else {
            x *= constants_.x_friction;
        }

        if (!physics_props.movement_locked_y_) {
            // Only apply jump multiplier if holding jump and moving upwards
            auto multiplier = (act->getActionState(Actions::Action::JUMP)
                               && y < 0.0) ? constants_.jump_multiplier : 1.0;
            y += constants_.gravity * multiplier;
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
                state->incomingEvent(state_utils::Event::DASHING);
            }
        }

        if (act->getActionState(Actions::Action::JUMP, true)) {
            if (physics_props.can_jump_ && physics_props.touching_ground_ && !physics_props.touching_wall_) {
                y = constants_.jump_impulse;
                state->incomingEvent(state_utils::Event::JUMPING);
            }
        }

        if (act->getActionState(Actions::Action::DOUBLE_JUMP, true)) {
            if (physics_props.can_jump_ &&
                !physics_props.touching_ground_ &&
                !physics_props.touching_wall_ &&
                variables_.popJumps()) {
                    y = constants_.jump_impulse;
                    state->incomingEvent(state_utils::Event::JUMPING);
            }
        }

        if (act->getActionState(Actions::Action::WALL_JUMP, true)) {
            if (physics_props.touching_wall_) {
                state->incomingEvent(state_utils::Event::JUMPING);
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

        if (act->getActionState(Actions::Action::AIR_DIVE, true) && physics_props.can_air_dive_) {
            state->incomingEvent(state_utils::Event::AIR_DIVING);
            x = 0.0;
            y = constants_.air_dive_impulse;
        }

        y *= constants_.y_friction;

        if (act->getActionState(Actions::Action::AIR_DIVE_BOUNCE, true)) {
            y = -30;
            state->incomingEvent(state_utils::Event::DIVE_BOUNCE);
            variables_.resetJumps();
            variables_.resetDashes();
        }

        move->setFacingRight(facing_right);
        move->setVelocity(x, y);
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

void Physics::setPhysicsVariables() {
    variables_.setMaxJumps(1);
    variables_.setMaxDashes(1);
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

    loadConstant(jump_impulse);
    loadConstant(wall_jump_horizontal_impulse);
    loadConstant(wall_jump_vertical_impulse);
    loadConstant(dash_speed);
    loadConstant(dash_friction);
    loadConstant(air_dive_impulse);

    return constants;
}

void Physics::reloadFromJson(nlohmann::json j, File /* file_instance */) {
    original_constants_ = loadConstantsFromJson(j, {});
    setPhysicsConstants(original_constants_);
    // TODO Read from json
    setPhysicsVariables();
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

    saveConstantToJson(j, "jump_impulse", original_constants_.jump_impulse, default_constants.jump_impulse);
    saveConstantToJson(j, "wall_jump_horizontal_impulse", original_constants_.wall_jump_horizontal_impulse, default_constants.wall_jump_horizontal_impulse);
    saveConstantToJson(j, "wall_jump_vertical_impulse", original_constants_.wall_jump_vertical_impulse, default_constants.wall_jump_vertical_impulse);
    saveConstantToJson(j, "dash_speed", original_constants_.dash_speed, default_constants.dash_speed);
    saveConstantToJson(j, "dash_friction", original_constants_.dash_friction, default_constants.dash_friction);
    saveConstantToJson(j, "air_dive_impulse", original_constants_.air_dive_impulse, default_constants.air_dive_impulse);

    return j;
}
