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
               std::weak_ptr<RenderableEntity> render,
               std::weak_ptr<MovableEntity> move);

    // Component interface
    virtual void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    // Load derived classes from json
    static std::shared_ptr<Damageable> createFromJson(nlohmann::json j,
                                                      std::weak_ptr<Collision> coll,
                                                      std::weak_ptr<Death> death,
                                                      std::weak_ptr<StatefulEntity> state,
                                                      std::weak_ptr<RenderableEntity> render,
                                                      std::weak_ptr<MovableEntity> move);

    int getHealth();
    bool isDead();

  protected:
    int health_ = 0;

    std::weak_ptr<Collision> hurtbox_;
    std::weak_ptr<Death> death_;
    std::weak_ptr<StatefulEntity> state_;
    std::weak_ptr<RenderableEntity> render_;
    std::weak_ptr<MovableEntity> move_;

  private:
    int invincibility_frame_counter_ = 0;

    // For output to json
    int initial_health_;
};
