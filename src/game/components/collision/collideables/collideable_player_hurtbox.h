#pragma once

#include "components/collision/collision.h"

class CollideablePlayerHurtbox : public Collideable {
  public:
    CollideablePlayerHurtbox(std::weak_ptr<Transform> trans, std::weak_ptr<Actions> actions);

    void update() override;

    CollisionType getType() const override;

  private:
    std::weak_ptr<Actions> actions_;
};

