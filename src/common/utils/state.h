#pragma once

namespace state {

enum class Event {
    // Initial entrypoint event
    START = 0,

    // Positions
    TOUCHING_FLOOR = 1,

    // Movement
    NO_MOVEMENT = 10,
    MOVING = 11,
    JUMPING = 12,
};

// TODO Remove
enum class Property {
    UNKNOWN,
    TOUCHING_GROUND,
    TOUCHING_RIGHT_WALL,
    TOUCHING_LEFT_WALL,
    MOVEMENT_LOCKED,
    CAN_JUMP
};

struct InitParams {
    bool movement_locked = false;
    bool touching_ground = true;
    bool touching_right_wall = false;
    bool touching_left_wall = false;
    bool can_jump = true;
    unsigned int frame_timer = UINT_MAX;
    std::unordered_map<state::Event, std::string> next_states;
    std::vector<std::string> frame_names;
};

}
