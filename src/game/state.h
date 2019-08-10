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

    static State loadStateFromJson(nlohmann::json j);

    std::optional<std::string> incomingEvent(state::Event event);

    const state::Properties properties_;
};
