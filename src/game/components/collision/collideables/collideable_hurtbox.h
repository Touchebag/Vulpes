#pragma once

#include <set>

#include "components/collision/collision.h"

class CollideableHurtbox : public Collideable {
  public:
    explicit CollideableHurtbox(std::weak_ptr<ComponentStore> components);

    virtual void reloadFromJson(nlohmann::json j) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    CollisionType getType() const override;

    const std::set<int> getTeams() const;

  private:
    std::set<int> teams_;
};

