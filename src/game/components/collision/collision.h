#pragma once

#include <memory>

#include "components/component.h"
#include "components/transform.h"
#include "hitbox.h"

class Collision : public Component {
  public:
    enum class CollisionType {
        STATIC,
        SEMI_SOLID,
        PLAYER_HURTBOX,
        PLAYER_HITBOX,
        ENEMY_HITBOX,
        TRANSITION,
        HEALTH,
        COLLECTIBLE,
        INTERACTABLE,

        MAX_NUM
    };

    Collision(std::weak_ptr<Transform> trans);

    // Component interface
    void update() override;

    virtual void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    // Load derived classes from json
    static std::shared_ptr<Collision> createFromJson(nlohmann::json j, std::weak_ptr<Transform> trans);

    // Collision interface
    bool collides(std::weak_ptr<const Collision> other_entity);

    // Getters/setters
    void setHitbox(int width, int height);
    const std::shared_ptr<const Hitbox> getHitbox() const;

    std::weak_ptr<const Transform> getTransform() const;

    // Type-specific functions
    virtual CollisionType getType() const = 0;

  protected:
    std::weak_ptr<Transform> trans_;

    std::shared_ptr<Hitbox> hbox_;
};
