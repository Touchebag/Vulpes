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

    bool hasTriggered();

  private:
    bool triggered_ = false;

    std::string name_;
};

