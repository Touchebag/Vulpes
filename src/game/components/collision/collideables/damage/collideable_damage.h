#pragma once

#include "components/collision/collideable.h"
#include "components/collision/attack_attributes.h"

class CollideableDamage : public Collideable {
  public:
    CollideableDamage(std::weak_ptr<ComponentStore> components);

    const collision::AttackAttributes getAttributes() const;

  protected:
    collision::AttackAttributes attack_attributes_;
};
