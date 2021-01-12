#pragma once

#include "actions.h"

class ActionsPlayer : public Actions {
  public:
    ActionsPlayer(std::weak_ptr<Death> death);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    void addAction(Action action) override;
};
