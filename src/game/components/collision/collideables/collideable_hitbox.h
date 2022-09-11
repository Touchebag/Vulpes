#pragma once

#include "components/collision/collideable.h"
#include "components/collision/attack_attributes.h"

class CollideableHitbox : public Collideable {
  public:
    explicit CollideableHitbox(std::weak_ptr<ComponentStore> components);

    virtual void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;

    const collision::AttackAttributes getAttributes() const;
  protected:
    collision::AttackAttributes attack_attributes_;
};
