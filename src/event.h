#pragma once

namespace state {

// TODO Unify all state namespaces
enum class Event {
    UNKNOWN = 0,
    TOUCHING_FLOOR = 1,
    TOUCHING_WALL_LEFT = 2,
    TOUCHING_WALL_RIGHT = 3,
    JUMP = 4,
    FRAME_TIMEOUT = 5
};

}
