#pragma once

#include <json.hpp>

#include "utils/state.h"
#include "components/animation.h"
#include "utils/state_handler.h"

class StatefulEntity {
  public:
    StatefulEntity(std::weak_ptr<AnimatedEntity> animatedEntity);

    void update();

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

    void incomingEvent(state_utils::Event event);

    const state_utils::Properties& getStateProperties();

  private:
    void loadStates(std::string file_path);

    unsigned int frame_counter_;

    std::string state_file_path_;

    std::weak_ptr<AnimatedEntity> animatedEntity_;

    StateHandler<state_utils::Properties> state_handler_;
};
