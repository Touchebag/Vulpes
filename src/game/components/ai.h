#pragma once

#include "components/actions.h"
#include "components/transform.h"

#include "utils/state_handler.h"
#include "ai/logic_operators/logic_operator.h"

class AI : public Component{
  public:
    AI(std::weak_ptr<Actions> actions, std::weak_ptr<Transform> transform, std::weak_ptr<AnimatedEntity> animated_entitiy);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

  private:
    std::weak_ptr<Actions> actions_;
    std::weak_ptr<Transform> transform_;
    std::weak_ptr<AnimatedEntity> animated_entitiy_;

    unsigned int frame_timer_ = 0;

    // For output to json
    std::string file_name_;

    StateHandler<std::vector<std::pair<std::shared_ptr<const ai::condition::LogicalOperator>, Actions::Action>>> states_;
};
