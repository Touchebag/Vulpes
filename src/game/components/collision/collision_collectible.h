#pragma once

#include "collision.h"

class CollisionCollectible : public Collision {
  public:
    CollisionCollectible(std::weak_ptr<Transform> trans);

    void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    Collision::CollisionType getType() const override;

    int getId() const;

  private:
    int id_;
};
