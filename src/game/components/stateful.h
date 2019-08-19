#pragma once

#include <json.hpp>

#include "state.h"
#include "components/animation.h"

class StatefulEntity {
  public:
    StatefulEntity(std::weak_ptr<AnimatedEntity> animatedEntity);

    void update();

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

    void incomingEvent(state::Event event);

    virtual const state::Properties& getStateProperties() final;

  private:
    void loadStates(std::string file_path);

    unsigned int frame_counter_;

    std::string state_file_path_;

    std::weak_ptr<AnimatedEntity> animatedEntity_;

    State* current_state_;
    std::unordered_map<std::string, State> state_list_;
};
