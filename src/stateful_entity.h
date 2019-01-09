#pragma once

#include "state.h"

// TODO Move to utils?
namespace state {

enum class Property {
    UNKNOWN,
    TOUCHING_GROUND,
    TOUCHING_RIGHT_WALL,
    TOUCHING_LEFT_WALL,
    MOVEMENT_LOCKED
};

}

class StatefulEntity {
  public:
    StatefulEntity();

    void incomingEvent(state::Event event);

    bool getProperty(state::Property property);
  private:
    State* current_state_;
    std::unordered_map<int, State> state_list_;
};
