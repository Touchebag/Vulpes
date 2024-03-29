#pragma once

#include "nlohmann/json.hpp"

#include "state.h"

class ComponentStore;

template <class T>
class StateHandler {
  public:
    void reloadFromJson(const nlohmann::json& j, std::shared_ptr<ComponentStore> components);
    void resetState();

    std::weak_ptr<State<T>> incomingEvent(state_utils::Event event);

    const T& getStateData();

  private:
    std::shared_ptr<State<T>> current_state_;
    std::unordered_map<std::string, std::shared_ptr<State<T>>> state_list_;
};
