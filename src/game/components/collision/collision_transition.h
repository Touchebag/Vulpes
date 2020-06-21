#pragma once

#include "collision.h"

class CollisionTransition : public Collision {
  public:
    CollisionTransition(std::weak_ptr<Transform> trans);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;

  private:
    int entrance_id_;
    std::string room_;
};

