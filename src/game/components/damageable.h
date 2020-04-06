#pragma once

#include <memory>

#include "json.hpp"

#include "components/collision.h"

class Damageable {
  public:
    Damageable(std::weak_ptr<Collision> hurtbox);

    void reloadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

    void update();

    int getHealth();
    bool isDead();

  private:
    int health_ = 0;

    std::weak_ptr<Collision> hurtbox_;
};
