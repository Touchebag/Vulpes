#pragma once

#include "components/collision/collision.h"

class CollideableSensor : public Collideable {
  public:
    explicit CollideableSensor(std::weak_ptr<ComponentStore> components);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;

    std::string getName();

    std::shared_ptr<const Collideable> getLastCollideable();

    bool hasTriggered();

  private:
    // Keep the previous collideable for scripting access
    std::weak_ptr<const Collideable> last_trigger_coll_;

    CollisionType target_type_;

    std::string name_;
};

