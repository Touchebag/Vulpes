#pragma once

#include "components/collision/collision.h"

class CollideableHurtbox : public Collideable {
  public:
    explicit CollideableHurtbox(std::weak_ptr<ComponentStore> components);

    CollisionType getType() const override;
};

