#pragma once

#include <memory>

#include "json.hpp"

#include "components/collision.h"
#include "components/death.h"

class Damageable : public Component {
  public:
    Damageable(std::weak_ptr<Collision> hurtbox, std::weak_ptr<Death> death);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    int getHealth();
    bool isDead();

  private:
    int health_ = 0;

    std::weak_ptr<Collision> hurtbox_;
    std::weak_ptr<Death> death_;
};
