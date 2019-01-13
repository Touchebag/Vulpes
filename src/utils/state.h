#pragma once

namespace state {

enum class Event {
    UNKNOWN = 0,
    TOUCHING_FLOOR = 1,
    TOUCHING_WALL_LEFT = 2,
    TOUCHING_WALL_RIGHT = 3,
    JUMP = 4,
    FRAME_TIMEOUT = 5
};

enum class Property {
    UNKNOWN,
    TOUCHING_GROUND,
    TOUCHING_RIGHT_WALL,
    TOUCHING_LEFT_WALL,
    MOVEMENT_LOCKED
};

struct InitParams {
    bool movement_locked = false;
    bool touching_ground = true;
    bool touching_right_wall = false;
    bool touching_left_wall = false;
    unsigned int frame_timer = UINT_MAX;
    std::unordered_map<state::Event, int> next_states;
};

}
