#pragma once

#include "base_entity.h"
#include "components/movement.h"
#include "components/stateful.h"
#include "components/animation.h"

class Player : public BaseEntity {
  public:
    static std::shared_ptr<Player> createFromJson(const nlohmann::json& j);

    void reloadFromJson(const nlohmann::json& j) override;

    void update() override;
};
