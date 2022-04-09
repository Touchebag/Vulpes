#pragma once

#include "components/collision/collision.h"

#include "system/cutscene.h"

class CollideableInteractable : public Collideable {
  public:
    explicit CollideableInteractable(std::weak_ptr<ComponentStore> components);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    Collideable::CollisionType getType() const override;

  private:
    std::optional<std::pair<std::string, unsigned int>> transition_;
    std::shared_ptr<Cutscene> cutscene_;
    std::optional<std::string> cutscene_name_;
};
