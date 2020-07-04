#pragma once

#include "collision.h"

class CollisionHealth : public Collision {
  public:
    CollisionHealth(std::weak_ptr<Transform> trans);

    void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    int getHealth() const override;

    Collision::CollisionType getType() const override;

  private:
    int health_ = 0;
};
