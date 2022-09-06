#pragma once

#include <set>

#include "components/collision/collideable.h"
#include "components/collision/attack_attributes.h"

class CollideableHitbox : public Collideable {
  public:
    explicit CollideableHitbox(std::weak_ptr<ComponentStore> components);

    virtual void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;

    const collision::AttackAttributes getAttributes() const;

    const std::set<int> getTeams() const;

  protected:
    std::set<int> teams_;

    collision::AttackAttributes attack_attributes_;
};
