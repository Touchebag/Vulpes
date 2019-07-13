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
    State(state::Properties props);

    void update();

    static State loadStateFromJson(nlohmann::json j);

    std::optional<std::string> incomingEvent(state::Event event);

    std::string getCurrentSprite();

    const state::Properties properties_;

  private:
    unsigned int current_frame_ = 0;
};
