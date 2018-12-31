#pragma once

#include "state.h"

class StatefulEntity {
  public:
    StatefulEntity();

    void incomingEvent(state::Event event);

    bool isMovementLocked();
  private:
    State* current_state_;
    std::unordered_map<int, State> state_list_;
};
