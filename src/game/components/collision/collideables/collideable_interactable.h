#pragma once

#include "components/collision/collision.h"

class CollisionInteractable : public Collision {
  public:
    CollisionInteractable(std::weak_ptr<Transform> trans);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    Collision::CollisionType getType() const override;

  private:
    std::optional<std::pair<std::string, int>> transition_;
    std::optional<std::string> cutscene_;
};
