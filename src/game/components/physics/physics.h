#pragma once

#include <memory>
#include <json.hpp>

#include "components/stateful.h"
#include "components/rendering/rendering.h"
#include "components/actions/actions.h"
#include "components/animation.h"
#include "components/movement.h"
#include "components/collision/collision.h"

#include "components/physics/constants.h"
#include "components/physics/variables.h"

class Physics : public Component {
  public:
    Physics(std::weak_ptr<StatefulEntity> statefulEntity,
            std::weak_ptr<MovableEntity> movableEntity,
            std::weak_ptr<AnimatedEntity> animatedEntity,
            std::weak_ptr<Actions> actions);

    void update() override;

    void setPhysicsConstants(PhysicsConstants constants);
    void setPhysicsVariables();

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

  private:
    std::weak_ptr<StatefulEntity> statefulEntity_;
    std::weak_ptr<MovableEntity> movableEntity_;
    std::weak_ptr<AnimatedEntity> animatedEntity_;
    std::weak_ptr<Actions> actions_;

    PhysicsConstants constants_;
    PhysicsVariables variables_;
};
