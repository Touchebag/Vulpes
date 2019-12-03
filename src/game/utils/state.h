#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <climits>
#include <vector>

#include "utils/state_utils.h"
#include "json.hpp"

template <class T>
class State {
  public:
    State(T data);

    static State<T> loadStateFromJson(nlohmann::json j);

    std::optional<std::string> incomingEvent(state_utils::Event event);

    const T& getData();

  private:
    void loadNextStateListFromJson(nlohmann::json j);

    const T data_;
    std::map<state_utils::Event, std::string> next_states_;
};
