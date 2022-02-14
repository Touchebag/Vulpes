#pragma once

#include "components/collision/collision.h"

class CollideableTransition : public Collideable {
  public:
    explicit CollideableTransition(std::weak_ptr<ComponentStore> components);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;

  private:
    unsigned int entrance_id_ = 0;
    std::string room_;
};

