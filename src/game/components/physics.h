#pragma once

#include <memory>
#include <json.hpp>

#include "components/stateful.h"
#include "components/rendering.h"
#include "components/actions.h"
#include "components/animation.h"
#include "components/movement.h"
#include "components/collision.h"

class Physics {
  public:
    Physics(std::weak_ptr<StatefulEntity> statefulEntity,
            std::weak_ptr<RenderableEntity> renderableEntity,
            std::weak_ptr<MovableEntity> movableEntity,
            std::weak_ptr<AnimatedEntity> animatedEntity,
            std::weak_ptr<Actions> actions,
            std::weak_ptr<Collision> collision);

    void update();

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

  private:
    std::weak_ptr<StatefulEntity> statefulEntity_;
    std::weak_ptr<RenderableEntity> renderableEntity_;
    std::weak_ptr<MovableEntity> movableEntity_;
    std::weak_ptr<AnimatedEntity> animatedEntity_;
    std::weak_ptr<Actions> actions_;
    std::weak_ptr<Collision> collision_;
};
