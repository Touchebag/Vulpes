#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <climits>

#include "utils/state.h"
#include "json.hpp"

class State {
  public:
    State(state::InitParams params);

    static State loadStateFromJson(nlohmann::json j);

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
