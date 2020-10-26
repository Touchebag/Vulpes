#pragma once

#include "components/actions/actions.h"
#include "components/collision/collision.h"
#include "components/transform.h"

#include "utils/state_handler.h"
#include "ai/logic_operators/logic_operator.h"

class AI : public Component{
  public:
    AI(std::weak_ptr<Actions> actions,
       std::weak_ptr<Transform> transform,
       std::weak_ptr<Collision> collision,
       std::weak_ptr<AnimatedEntity> animated_entitiy);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

  private:
    std::weak_ptr<Actions> actions_;
    std::weak_ptr<Transform> transform_;
    std::weak_ptr<Collision> collision_;
    std::weak_ptr<AnimatedEntity> animated_entitiy_;

    unsigned int frame_timer_ = 0;

    StateHandler<std::vector<std::pair<std::shared_ptr<const ai::condition::LogicalOperator>, Actions::Action>>> states_;
};
