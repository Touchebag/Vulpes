#pragma once

#include <map>

#include "nlohmann/json.hpp"

#include "ai/program.h"
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
    NO_MOVEMENT,
    MOVING,
    JUMPING,
    FALLING,
    DASHING,
    AIR_DIVING,
    DIVE_BOUNCE,
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

    {"no_movement", Event::NO_MOVEMENT},
    {"moving", Event::MOVING},
    {"jumping", Event::JUMPING},
    {"falling", Event::FALLING},
    {"dashing", Event::DASHING},
    {"air_diving", Event::AIR_DIVING},
    {"dive_bounce", Event::DIVE_BOUNCE},
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
    bool dashing_ = false;
    bool air_diving_ = false;
    bool can_jump_ = true;
    bool can_dash_ = true;
    bool can_air_dive_ = false;
};

struct StateProperties {
    unsigned int frame_timer_ = UINT_MAX;
    std::string animation_name;

    nlohmann::json collideables;

    bool can_interact = false;
};

struct EntityContent {
    PhysicsProperties physics_props;
    StateProperties state_props;
    nlohmann::json entity;
    std::vector<Program> ai;
    std::shared_ptr<PhysicsConstants> physics_constants;
};

} // state_utils
