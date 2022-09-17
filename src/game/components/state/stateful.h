#pragma once

#include "state.h"
#include "state_handler.h"

#include "components/actions/actions.h"
#include "components/ai/program.h"
#include "components/animation.h"
#include "components/subentity.h"
#include "components/collision/collision.h"

class Stateful : public Component {
  public:
    explicit Stateful(std::weak_ptr<ComponentStore> components);

    void update() override;

    static std::shared_ptr<Stateful> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

    void resetState();

    void incomingEvent(state_utils::Event event);

    const state_utils::StateProperties& getStateProperties();
    const state_utils::PhysicsProperties& getPhysicsProperties();
    const nlohmann::json& getEntity();
    const std::vector<Program>& getAI();

  private:
    void loadStates(File file_instance);

    void checkTemporaryCollideables(const state_utils::StateProperties& state_props);

    unsigned int frame_counter_ = UINT_MAX;

    StateHandler<state_utils::EntityContent> state_handler_;
};
