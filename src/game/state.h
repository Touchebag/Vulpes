#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <climits>
#include <vector>

#include "utils/state.h"
#include "json.hpp"

class State {
  public:
    State(state::InitParams params);

    void update();

    static State loadStateFromJson(nlohmann::json j);

    std::optional<std::string> incomingEvent(state::Event event);

    std::string getCurrentSpriteName();

    // TODO Add name
    const bool movement_locked_;
    const bool touching_ground_;
    const bool touching_right_wall_;
    const bool touching_left_wall_;
    const unsigned int frame_timer_;

  private:
    const std::unordered_map<state::Event, std::string> next_state_list_;
    const std::vector<std::string> frame_names_;
    unsigned int current_frame_ = 0;
};
