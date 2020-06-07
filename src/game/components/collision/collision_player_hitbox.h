#pragma once

#include "collision.h"

class CollisionPlayerHitbox : public Collision {
  public:
    CollisionPlayerHitbox(std::weak_ptr<Transform> trans);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    const collision::AttackAttributes getAttributes() const override;

    CollisionType getType() const override;

  private:
    collision::AttackAttributes attack_attributes_;
};

