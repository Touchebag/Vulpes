#pragma once

#include "collision_damage.h"

class CollisionEnemyHitbox : public CollisionDamage {
  public:
    CollisionEnemyHitbox(std::weak_ptr<Transform> trans);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;
};

