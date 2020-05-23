#pragma once

#include <memory>

#include "component.h"

#include "components/transform.h"
#include "components/hitbox.h"

class Collision : public Component {
  public:
    enum class CollisionType {
        STATIC,
        PLAYER_HURTBOX,
        PLAYER_HITBOX,
        ENEMY_HITBOX,

        MAX_NUM
    };

    Collision(std::weak_ptr<Transform> trans, std::weak_ptr<Hitbox> hbox);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    bool collides(std::weak_ptr<const Collision> other_entity);

    std::pair<double, double> getMaximumMovement(double stepX, double stepY, std::shared_ptr<const Collision> other_coll);

    CollisionType getType() const;

  private:
    std::pair<double, double> getMaximumMovement(double stepX, double stepY,
            std::shared_ptr<Transform> other_trans, std::shared_ptr<Hitbox> other_hbox);

    std::weak_ptr<Transform> trans_;
    std::weak_ptr<Hitbox> hbox_;

    CollisionType type_ = CollisionType::STATIC;
};
