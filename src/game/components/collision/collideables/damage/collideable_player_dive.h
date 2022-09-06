#pragma once

#include "components/collision/collideables/collideable_hitbox.h"
#include "components/component_store.h"

class CollideablePlayerDive : public CollideableHitbox {
  public:
    explicit CollideablePlayerDive(std::weak_ptr<ComponentStore> components);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;
};

