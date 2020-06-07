#pragma once

#include <memory>

#include "components/component.h"
#include "components/transform.h"
#include "hitbox.h"
#include "attack_attributes.h"

class Collision : public Component {
  public:
    enum class CollisionType {
        STATIC,
        PLAYER_HURTBOX,
        PLAYER_HITBOX,
        ENEMY_HITBOX,

        MAX_NUM
    };

    Collision(std::weak_ptr<Transform> trans);

    // Component interface
    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    // Load derived classes from json
    static std::shared_ptr<Collision> createFromJson(nlohmann::json j, std::weak_ptr<Transform> trans);

    // Collision interface
    bool collides(std::weak_ptr<const Collision> other_entity);

    std::pair<double, double> getMaximumMovement(double stepX, double stepY, std::shared_ptr<const Collision> other_coll);

    // Getters/setters
    void setHitbox(int width, int height);
    const std::shared_ptr<Hitbox>& getHitbox() const;

    std::weak_ptr<const Transform> getTransform() const;

    virtual const collision::AttackAttributes getAttributes() const;

    virtual CollisionType getType() const = 0;

  protected:
    std::weak_ptr<Transform> trans_;

    std::shared_ptr<Hitbox> hbox_;

  private:
    std::pair<double, double> getMaximumMovement(double stepX, double stepY,
            std::shared_ptr<Transform> other_trans, std::shared_ptr<Hitbox> other_hbox);
};
