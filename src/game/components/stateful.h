#pragma once

#include <json.hpp>

#include "state.h"
#include "components/animation.h"
#include "utils/state_handler.h"

class StatefulEntity {
  public:
    StatefulEntity(std::weak_ptr<AnimatedEntity> animatedEntity);

    void update();

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

    void incomingEvent(state::Event event);

    const state::Properties& getStateProperties();

  private:
    void loadStates(std::string file_path);

    unsigned int frame_counter_;

    std::string state_file_path_;

    std::weak_ptr<AnimatedEntity> animatedEntity_;

    StateHandler state_handler_;
};
