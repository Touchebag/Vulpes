#pragma once

#include <memory>
#include <json.hpp>

#include "attributes/stateful.h"
#include "attributes/renderable.h"
#include "attributes/actions.h"
#include "attributes/animated.h"
#include "attributes/movable.h"
#include "attributes/collision.h"

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
