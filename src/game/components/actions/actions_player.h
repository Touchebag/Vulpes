#pragma once

#include "actions.h"

#include "components/stateful.h"

class ActionsPlayer : public Actions {
  public:
    ActionsPlayer(std::weak_ptr<Death> death, std::weak_ptr<Collision> coll, std::weak_ptr<StatefulEntity> state);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    void addAction(Action action) override;

  private:
    std::weak_ptr<Collision> coll_;
    std::weak_ptr<StatefulEntity> state_;
};
