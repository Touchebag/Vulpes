#pragma once

#include "components/actions.h"
#include "components/transform.h"

class AI {
  public:
    AI(std::weak_ptr<Actions> actions, std::weak_ptr<Transform> transform);

    void update();

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

  private:
    std::weak_ptr<Actions> actions_;
    std::weak_ptr<Transform> transform_;
};
