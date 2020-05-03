#pragma once

#include <json.hpp>

#include "utils/state.h"
#include "components/animation.h"
#include "utils/state_handler.h"

class StatefulEntity : public Component {
  public:
    StatefulEntity(std::weak_ptr<AnimatedEntity> animatedEntity);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void incomingEvent(state_utils::Event event);

    const state_utils::Properties& getStateProperties();

  private:
    void loadStates(std::string file_path);

    unsigned int frame_counter_;

    std::string state_file_path_;

    std::weak_ptr<AnimatedEntity> animatedEntity_;

    StateHandler<state_utils::Properties> state_handler_;
};
