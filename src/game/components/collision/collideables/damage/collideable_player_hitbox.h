#pragma once

#include "components/collision/collideable.h"
#include "collideable_damage.h"

class CollideablePlayerHitbox : public CollideableDamage {
  public:
    CollideablePlayerHitbox(std::weak_ptr<Transform> trans);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;
};

