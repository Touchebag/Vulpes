#pragma once

#include "components/actions.h"

class AI {
  public:
    AI(std::weak_ptr<Actions> actions);

    void update();

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

  private:
    std::weak_ptr<Actions> actions_;
};
