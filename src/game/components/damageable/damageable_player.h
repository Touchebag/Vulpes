#pragma once

#include "damageable.h"

class DamageablePlayer : public Damageable {
  public:
    explicit DamageablePlayer(std::weak_ptr<ComponentStore> components);

    virtual std::optional<nlohmann::json> outputToJson() override;

    void update() override;
};
