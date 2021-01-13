#pragma once

#include "components/collision/collideable.h"
#include "components/actions/actions.h"
#include "collideable_damage.h"

class CollideablePlayerDive : public CollideableDamage {
  public:
    CollideablePlayerDive(std::weak_ptr<Transform> trans, std::weak_ptr<Actions> actions);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;

  private:
    std::weak_ptr<Actions> actions_;
};

