#pragma once

#include "state.h"

class StatefulEntity {
  public:
    StatefulEntity();

    void incomingEvent(state::Event event);

    bool getProperty(state::Property property);

  protected:
    unsigned int frame_counter_;

  private:
    State* current_state_;
    std::unordered_map<int, State> state_list_;
};
