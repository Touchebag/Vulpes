#pragma once

#include "components/collision/collision.h"

class CollideableInteractable : public Collideable {
  public:
    CollideableInteractable(std::weak_ptr<ComponentStore> components);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    Collideable::CollisionType getType() const override;

  private:
    std::optional<std::pair<std::string, int>> transition_;
    std::optional<std::string> cutscene_;
};
