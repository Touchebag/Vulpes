#pragma once

#include "components/collision/collideable.h"
#include "collideable_damage.h"

class CollideablePlayerHitbox : public CollideableDamage {
  public:
    explicit CollideablePlayerHitbox(std::weak_ptr<ComponentStore> components);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;
};

