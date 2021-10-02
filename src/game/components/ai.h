#pragma once

#include "components/actions/actions.h"
#include "components/collision/collision.h"
#include "components/transform.h"

#include "ai/program.h"

#include "utils/state_handler.h"
#include "utils/state.h"

class AI : public Component {
  public:
    AI(std::weak_ptr<ComponentStore> components);

    void update() override;

    static std::shared_ptr<AI> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file_instance = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

    StateHandler<std::vector<STATE_AI_CONDITION_TYPE>> states_;

  private:
    unsigned int frame_timer_ = 0;
};
