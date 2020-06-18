#pragma once

#include <memory>

#include "json.hpp"

#include "components/collision/collision.h"
#include "components/death.h"
#include "components/stateful.h"
#include "components/movement.h"

class Damageable : public Component {
  public:
    Damageable(std::weak_ptr<Collision> hurtbox,
               std::weak_ptr<Death> death,
               std::weak_ptr<StatefulEntity> state,
               std::weak_ptr<MovableEntity> move);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    int getHealth();
    bool isDead();

  private:
    int health_ = 0;

    int invincibility_frame_counter_;

    // For output to json
    int initial_health_;

    std::weak_ptr<Collision> hurtbox_;
    std::weak_ptr<Death> death_;
    std::weak_ptr<StatefulEntity> state_;
    std::weak_ptr<MovableEntity> move_;
};
