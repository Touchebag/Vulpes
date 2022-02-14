#pragma once

#include <memory>
#include <unordered_map>

#include "components/component.h"
#include "components/transform.h"
#include "components/actions/actions.h"
#include "hitbox.h"
#include "collideable.h"

class CollideableSensor;

class Collision : public Component {
  public:
    explicit Collision(std::weak_ptr<ComponentStore> components);

    // Component interface
    void update() override;

    // Load derived classes from json
    static std::shared_ptr<Collision> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());

    void reloadFromJson(nlohmann::json j, File file = File()) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    // Collision interface
    bool collides(std::weak_ptr<const Collision> other_entity);
    bool collides(std::weak_ptr<const Collideable> other_entity);

    // Getters/setters
    std::weak_ptr<const Transform> getTransform() const;

    std::shared_ptr<Collideable> getCollideable() const;
    void setCollideable(nlohmann::json j);

    void addTemporaryCollideable(nlohmann::json temp_coll);
    void clearTemporaryCollideables();

    bool isSensorTriggered(std::string sensor_name);

    // Type-specific functions
    virtual Collideable::CollisionType getType() const;

  protected:
    std::shared_ptr<Collideable> collideable_;

    std::vector<std::shared_ptr<Collideable>> temp_colls_;

    std::unordered_map<std::string, std::weak_ptr<CollideableSensor>> sensor_colls_;
};
