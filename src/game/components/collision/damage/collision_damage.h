#pragma once

#include "components/collision/collision.h"
#include "components/collision/attack_attributes.h"

class CollisionDamage : public Collision {
  public:
    CollisionDamage(std::weak_ptr<Transform> trans);

    const collision::AttackAttributes getAttributes() const;

  protected:
    collision::AttackAttributes attack_attributes_;
};
