#pragma once

#include "components/collision/collision.h"
#include "collision_damage.h"

class CollisionPlayerHitbox : public CollisionDamage {
  public:
    CollisionPlayerHitbox(std::weak_ptr<Transform> trans);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;
};

