#pragma once

#include <memory>

#include "components/component.h"
#include "components/transform.h"
#include "components/actions/actions.h"
#include "hitbox.h"
#include "collideable.h"

class Collision : public Component {
  public:
    Collision(std::weak_ptr<Transform> trans, std::weak_ptr<Actions> actions);

    // Component interface
    void update() override;

    virtual void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    // Load derived classes from json
    static std::shared_ptr<Collision> createFromJson(nlohmann::json j, std::weak_ptr<Transform> trans, std::weak_ptr<Actions> actions);

    // Collision interface
    bool collides(std::weak_ptr<const Collision> other_entity);
    bool collides(std::weak_ptr<const Collideable> other_entity);

    // Getters/setters
    std::weak_ptr<const Transform> getTransform() const;

    // Type-specific functions
    virtual Collideable::CollisionType getType() const;

    std::shared_ptr<Collideable> getCollideable() const;
    void setCollideable(nlohmann::json j);

  protected:
    std::shared_ptr<Collideable> collideable_;

    std::weak_ptr<Transform> trans_;
    std::weak_ptr<Actions> actions_;
};
