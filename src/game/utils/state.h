#pragma once

namespace state {

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

struct Properties {
    bool movement_locked_x_ = false;
    bool movement_locked_y_ = false;
    bool touching_ground_ = true;
    bool touching_wall_ = false;
    bool can_jump_ = true;
    bool can_dash_ = true;
    unsigned int frame_timer_ = UINT_MAX;
    std::unordered_map<state::Event, std::string> next_states_;
    std::vector<std::string> frame_names_;
};

}
