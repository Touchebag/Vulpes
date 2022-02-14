#pragma once

#include "components/collision/collideable.h"
#include "components/actions/actions.h"
#include "collideable_damage.h"

class CollideablePlayerDive : public CollideableDamage {
  public:
    explicit CollideablePlayerDive(std::weak_ptr<ComponentStore> components);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;
};

