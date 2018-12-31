#pragma once

#include <memory>
#include <optional>
#include <unordered_map>

#include "event.h"

namespace state {
    struct InitParams {
        bool movement_locked = false;
        unsigned int frame_timer = 0;
        std::unordered_map<state::Event, int> next_states;
    };
}

class State {
  public:
    State(state::InitParams params);

    std::optional<int> incomingEvent(state::Event event);

    const bool movement_locked_;
    const unsigned int frame_timer_;

  private:
    std::unordered_map<state::Event, int> next_state_list_;
};
