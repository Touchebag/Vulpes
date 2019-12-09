#pragma once

#include <map>

namespace state_utils {

enum class Event {
    // Initial entrypoint event
    START = 0,

    // Positions
    TOUCHING_FLOOR = 1,
    TOUCHING_WALL = 2,

    // Movement
    NO_MOVEMENT = 10,
    MOVING = 11,
    JUMPING = 12,
    FALLING = 13,
    DASHING = 14,

    // Misc
    FRAME_TIMEOUT = 20,
};

const std::map<std::string, Event> string_event_map {
    {
        {"start", Event::START},

        {"touching_floor", Event::TOUCHING_FLOOR},
        {"touching_wall", Event::TOUCHING_WALL},

        {"no_movement", Event::NO_MOVEMENT},
        {"moving", Event::MOVING},
        {"jumping", Event::JUMPING},
        {"falling", Event::FALLING},
        {"dashing", Event::DASHING},

        {"frame_timeout", Event::FRAME_TIMEOUT},
    }
};

struct Properties {
    bool movement_locked_x_ = false;
    bool movement_locked_y_ = false;
    bool touching_ground_ = true;
    bool touching_wall_ = false;
    bool dashing_ = false;
    bool can_jump_ = true;
    bool can_dash_ = true;
    unsigned int frame_timer_ = UINT_MAX;
    std::unordered_map<state_utils::Event, std::string> next_states_;
    std::vector<std::string> frame_names_;
};

}
