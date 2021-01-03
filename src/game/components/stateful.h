#pragma once

#include <json.hpp>

#include "utils/state.h"
#include "components/animation.h"
#include "components/subentity.h"
#include "utils/state_handler.h"

class StatefulEntity : public ComponentWithFile {
  public:
    StatefulEntity(std::weak_ptr<AnimatedEntity> animatedEntity, std::weak_ptr<Subentity> subentity);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    void reloadFromJson(nlohmann::json j, File file_instance) override;
    std::optional<nlohmann::json> outputToJson() override;

    void resetState();

    void incomingEvent(state_utils::Event event);

    const state_utils::StateProperties& getStateProperties();
    const state_utils::PhysicsProperties& getPhysicsProperties();
    const nlohmann::json& getEntity();

  private:
    void loadStates(File file_instance);

    unsigned int frame_counter_;

    std::weak_ptr<AnimatedEntity> animatedEntity_;
    std::weak_ptr<Subentity> subentity_;

    StateHandler<state_utils::EntityContent> state_handler_;
};
