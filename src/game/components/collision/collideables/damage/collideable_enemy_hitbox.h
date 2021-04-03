#pragma once

#include "collideable_damage.h"

class CollideableEnemyHitbox : public CollideableDamage {
  public:
    CollideableEnemyHitbox(std::weak_ptr<ComponentStore> components);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;
};

