#pragma once

#include <memory>
#include <set>

#include "components/component.h"
#include "components/actions/actions.h"
#include "components/transform.h"
#include "components/collision/hitbox.h"

class Collideable {
  public:
    enum class CollisionType {
        STATIC,
        SLOPE,

        PLAYER_DIVE,

        HITBOX,
        HURTBOX,

        TRANSITION,
        COLLECTIBLE,
        INTERACTABLE,

        SENSOR,

        MAX_NUM
    };

    inline static const std::map<std::string, Collideable::CollisionType> string_type_map {
        {"static", Collideable::CollisionType::STATIC},
        {"slope", Collideable::CollisionType::SLOPE},

        {"hurtbox", Collideable::CollisionType::HURTBOX},
        {"hitbox", Collideable::CollisionType::HITBOX},

        {"player_dive", Collideable::CollisionType::PLAYER_DIVE},

        {"transition", Collideable::CollisionType::TRANSITION},
        {"collectible", Collideable::CollisionType::COLLECTIBLE},
        {"interactable", Collideable::CollisionType::INTERACTABLE},

        {"sensor", Collideable::CollisionType::SENSOR},
    };

    Collideable(std::weak_ptr<ComponentStore> components);
    virtual ~Collideable() = default;

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

    const std::set<int> getTeams() const;

    void setDirectionMultiplier(int multiplier);

  protected:
    std::weak_ptr<ComponentStore> components_;

    std::shared_ptr<Hitbox> hbox_;

    std::set<int> teams_;
};
