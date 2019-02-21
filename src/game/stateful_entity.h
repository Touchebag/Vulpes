#pragma once

#include "state.h"

class StatefulEntity {
  public:
    StatefulEntity();

    void updateState();

    void incomingEvent(state::Event event);

    bool getProperty(state::Property property);

    std::pair<std::string, bool> getCurrentSprite();

  protected:
    unsigned int frame_counter_;

  private:
    State* current_state_;
    std::unordered_map<std::string, State> state_list_;
};
