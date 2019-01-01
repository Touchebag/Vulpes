#pragma once

namespace state {

// TODO Unify all state namespaces
enum class Event {
    UNKNOWN,
    TOUCHING_FLOOR,
    TOUCHING_WALL_RIGHT,
    TOUCHING_WALL_LEFT,
    JUMP
};

}
