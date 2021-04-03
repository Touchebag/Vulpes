#pragma once

#include <memory>

#include "components/component.h"
#include "components/actions/actions.h"
#include "components/transform.h"
#include "components/collision/hitbox.h"

class Collideable {
  public:
    enum class CollisionType {
        STATIC,
        SEMI_SOLID,
        SLOPE,
        PLAYER_HURTBOX,
        PLAYER_HITBOX,
        PLAYER_DIVE,
        ENEMY_HITBOX,
        TRANSITION,
        HEALTH,
        COLLECTIBLE,
        INTERACTABLE,

        MAX_NUM
    };

    Collideable(std::weak_ptr<ComponentStore> components);

    // Component interface
    virtual void update();

    virtual void reloadFromJson(nlohmann::json j);
    virtual std::optional<nlohmann::json> outputToJson();

    // Load derived classes from json
    static std::shared_ptr<Collideable> createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components);

    // Collision interface
    bool collides(std::weak_ptr<const Collideable> other_entity) const;
    bool collides(std::shared_ptr<const Transform> other_trans, std::shared_ptr<const Hitbox> other_hbox) const;

    // Getters/setters
    void setHitbox(int width, int height);
    const std::shared_ptr<const Hitbox> getHitbox() const;

    std::weak_ptr<const Transform> getTransform() const;

    // Type-specific functions
    virtual CollisionType getType() const = 0;

  protected:
    std::weak_ptr<ComponentStore> components_;

    std::shared_ptr<Hitbox> hbox_;
};
