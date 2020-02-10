#pragma once

#include "components/actions.h"
#include "components/transform.h"

#include "utils/state_handler.h"
#include "ai/logic_operators/logic_operator.h"

class AI {
  public:
    AI(std::weak_ptr<Actions> actions, std::weak_ptr<Transform> transform);

    void update();

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

  private:
    std::weak_ptr<Actions> actions_;
    std::weak_ptr<Transform> transform_;

    unsigned int frame_timer_ = 0;

    StateHandler<std::vector<std::pair<std::shared_ptr<const ai::condition::LogicalOperator>, Actions::Action>>> states_;
};
