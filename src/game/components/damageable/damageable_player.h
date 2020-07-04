#pragma once

#include "damageable.h"

class DamageablePlayer : public Damageable {
  public:
    DamageablePlayer(std::weak_ptr<Collision> hurtbox,
                     std::weak_ptr<Death> death,
                     std::weak_ptr<StatefulEntity> state,
                     std::weak_ptr<RenderableEntity> render,
                     std::weak_ptr<MovableEntity> move);

    virtual void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    void update() override;
};
