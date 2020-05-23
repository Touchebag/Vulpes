#pragma once

#include <memory>

#include "json.hpp"

#include "components/collision.h"

class Damageable : public Component {
  public:
    Damageable(std::weak_ptr<Collision> hurtbox, bool& active);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    int getHealth();
    bool isDead();

  private:
    int health_ = 0;

    std::weak_ptr<Collision> hurtbox_;

    bool& active_;
};
