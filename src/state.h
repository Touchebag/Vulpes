#pragma once

#include <memory>
#include <optional>
#include <unordered_map>

#include "event.h"

namespace state {
    struct InitParams {
        bool movement_locked = false;
        bool touching_ground = true;
        bool touching_right_wall = false;
        bool touching_left_wall = false;
        unsigned int frame_timer = 0;
        std::unordered_map<state::Event, int> next_states;
    };
}

class State {
  public:
    State(state::InitParams params);

    std::optional<int> incomingEvent(state::Event event);

    // TODO Add name
    const bool movement_locked_;
    const bool touching_ground_;
    const bool touching_right_wall_;
    const bool touching_left_wall_;
    const unsigned int frame_timer_;

  private:
    std::unordered_map<state::Event, int> next_state_list_;
};
