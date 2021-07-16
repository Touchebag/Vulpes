#pragma once

#include "components/collision/collision.h"

class CollideableCollectible : public Collideable {
  public:
    CollideableCollectible(std::weak_ptr<ComponentStore> components);

    void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    int getHealth() const;

    Collideable::CollisionType getType() const override;

  private:
    int health_ = 0;
};
