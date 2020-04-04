#pragma once

#include <memory>
#include <json.hpp>

#include "components/stateful.h"
#include "components/rendering/rendering.h"
#include "components/actions.h"
#include "components/animation.h"
#include "components/movement.h"
#include "components/collision.h"

class Physics {
  public:
    struct Constants {
        // Ground movement
        double ground_acceleration = 0.0;
        double ground_friction = 0.0;

        // Air movement
        double air_acceleration = 0.0;
        double air_friction = 0.0;
        double gravity = 0.0;
        double fall_multiplier = 0.0;
        double low_jump_multiplier = 0.0;
        double max_vertical_speed = 0.0;
        double min_vertical_speed = 0.0;

        // Other
        double jump_impulse = 0.0;
        double wall_slide_friction = 0.0;
        double wall_jump_horizontal_impulse = 0.0;
        double wall_jump_vertical_impulse = 0.0;
        double dash_speed = 0.0;
        double dash_friction = 0.0;
    };

    Physics(std::weak_ptr<StatefulEntity> statefulEntity,
            std::weak_ptr<RenderableEntity> renderableEntity,
            std::weak_ptr<MovableEntity> movableEntity,
            std::weak_ptr<AnimatedEntity> animatedEntity,
            std::weak_ptr<Actions> actions,
            std::weak_ptr<Collision> collision);

    void update();

    void setPhysicsConstants(Constants constants);

    void reloadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

  private:
    std::weak_ptr<StatefulEntity> statefulEntity_;
    std::weak_ptr<RenderableEntity> renderableEntity_;
    std::weak_ptr<MovableEntity> movableEntity_;
    std::weak_ptr<AnimatedEntity> animatedEntity_;
    std::weak_ptr<Actions> actions_;
    std::weak_ptr<Collision> collision_;

    Constants constants_;
};
