#pragma once

#include <map>

#include "nlohmann/json.hpp"

#include "components/scripting/program.h"
#include "components/physics/constants.h"

namespace state_utils {

enum class Event {
    // Initial entrypoint event
    START,

    // Positions
    TOUCHING_FLOOR,
    TOUCHING_WALL,
    AIRBORNE,

    // Movement
    NO_MOVEMENT_X,
    NO_MOVEMENT_Y,
    MOVING_X,
    MOVING_Y,
    JUMPING,
    FALLING,
    DASHING,
    LEDGE_CLIMB,

    #define GENERATE_ENUM(action, string) ACTION_##action,
    #include "components/actions/actions_enum.h"
    #undef GENERATE_ENUM

    // Effects
    DAMAGED,

    // Misc
    FRAME_TIMEOUT,
};

const std::map<std::string, Event> string_event_map {
    {"start", Event::START},

    {"touching_floor", Event::TOUCHING_FLOOR},
    {"touching_wall", Event::TOUCHING_WALL},
    {"airborne", Event::AIRBORNE},

    {"no_movement_x", Event::NO_MOVEMENT_X},
    {"no_movement_y", Event::NO_MOVEMENT_Y},
    {"moving_x", Event::MOVING_X},
    {"moving_y", Event::MOVING_Y},
    {"jumping", Event::JUMPING},
    {"falling", Event::FALLING},
    {"dashing", Event::DASHING},
    {"ledge_climb", Event::LEDGE_CLIMB},

    #define GENERATE_ENUM(action, string) {"action_" string, Event::ACTION_##action},
    #include "components/actions/actions_enum.h"
    #undef GENERATE_ENUM

    {"damaged", Event::DAMAGED},

    {"frame_timeout", Event::FRAME_TIMEOUT},
};

struct PhysicsProperties {
    bool movement_locked_x_ = false;
    bool movement_locked_y_ = false;
    bool direction_locked_ = false;
    bool touching_ground_ = true;
    bool touching_wall_ = false;
};

struct StateProperties {
    unsigned int frame_timer_ = UINT_MAX;
    std::string animation_name;

    nlohmann::json collideables;
};

struct EntityContent {
    PhysicsProperties physics_props;
    StateProperties state_props;
    nlohmann::json entity;
    std::vector<Program> script;
    std::vector<Program> script_on_enter;
    std::vector<Program> script_on_exit;
    std::optional<PhysicsConstants> physics_constants;
};

} // state_utils
