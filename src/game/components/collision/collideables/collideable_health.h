#pragma once

#include "components/collision/collision.h"

class CollideableHealth : public Collideable {
  public:
    CollideableHealth(std::weak_ptr<ComponentStore> components);

    void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    int getHealth() const;

    Collideable::CollisionType getType() const override;

  private:
    int health_ = 0;
};
