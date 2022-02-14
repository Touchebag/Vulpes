#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <climits>
#include <vector>

#include "state_utils.h"
#include "nlohmann/json.hpp"

class ComponentStore;

template <class T>
class State {
  public:
    explicit State(T data);

    static State<T> loadStateFromJson(nlohmann::json j, std::shared_ptr<ComponentStore> components);

    std::optional<std::string> incomingEvent(state_utils::Event event);

    const T& getData();

  private:
    void loadNextStateListFromJson(nlohmann::json j);

    const T data_;
    std::unordered_map<state_utils::Event, std::string> next_states_;
};
