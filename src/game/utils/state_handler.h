#include <json.hpp>

#include "state.h"

class StateHandler {
  public:
    void loadFromJson(nlohmann::json& j);

    std::weak_ptr<State> incomingEvent(state::Event event);

    const state::Properties& getStateData();

  private:
    std::shared_ptr<State> current_state_;
    std::unordered_map<std::string, std::shared_ptr<State>> state_list_;
};
