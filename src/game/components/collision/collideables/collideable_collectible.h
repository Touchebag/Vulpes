#pragma once

#include "components/collision/collision.h"

class CollideableCollectible : public Collideable {
  public:
    CollideableCollectible(std::weak_ptr<Transform> trans);

    void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    Collideable::CollisionType getType() const override;

    int getId() const;

  private:
    int id_;
};
