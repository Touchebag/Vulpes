#pragma once

#include "base_entity.h"

class Player : public BaseEntity {
  public:
    static std::shared_ptr<Player> createFromJson(const nlohmann::json& j);

    void reloadFromJson(const nlohmann::json& j) override;

    void update() override;
};
