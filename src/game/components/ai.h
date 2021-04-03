#pragma once

#include "components/actions/actions.h"
#include "components/collision/collision.h"
#include "components/transform.h"

#include "utils/state_handler.h"
#include "ai/logic_operators/logic_operator.h"

class AI : public Component {
  public:
    AI(std::weak_ptr<ComponentStore> components);

    void update() override;

    static std::shared_ptr<AI> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file_instance = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

  private:
    unsigned int frame_timer_ = 0;

    StateHandler<std::vector<std::pair<std::shared_ptr<const ai::condition::LogicalOperator>, Actions::Action>>> states_;
};
