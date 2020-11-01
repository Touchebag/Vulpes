#pragma once

#include <map>

#include "json.hpp"

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

    // Actions
    ATTACKING,

    // Effects
    DAMAGED,

    // Misc
    FRAME_TIMEOUT,

    // AI events
    AI_EVENT_1,
    AI_EVENT_2,
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

    {"attacking", Event::ATTACKING},

    {"damaged", Event::DAMAGED},

    {"frame_timeout", Event::FRAME_TIMEOUT},

    {"ai_event_1", Event::AI_EVENT_1},
    {"ai_event_2", Event::AI_EVENT_2},
};

struct PhysicsProperties {
    bool movement_locked_x_ = false;
    bool movement_locked_y_ = false;
    bool direction_locked_ = false;
    bool touching_ground_ = true;
    bool touching_wall_ = false;
    bool dashing_ = false;
    bool can_jump_ = true;
    bool can_dash_ = true;
};

struct StateProperties {
    unsigned int frame_timer_ = UINT_MAX;
    std::unordered_map<state_utils::Event, std::string> next_states_;
    std::string animation_name;

    bool can_interact = false;
};

struct EntityContent {
    PhysicsProperties physics_props;
    StateProperties state_props;
    nlohmann::json entity;
};

} // state_utils
