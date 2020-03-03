#pragma once

#include "base_entity.h"
#include "components/movement.h"
#include "components/stateful.h"
#include "components/animation.h"

class Player : public BaseEntity {
  public:
    static std::shared_ptr<Player> createFromJson(nlohmann::json j);

    void reloadFromJson(nlohmann::json j) override;

    void update() override;
};
