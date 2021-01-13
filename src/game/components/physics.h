#pragma once

#include <memory>
#include <json.hpp>

#include "components/stateful.h"
#include "components/rendering/rendering.h"
#include "components/actions/actions.h"
#include "components/animation.h"
#include "components/movement.h"
#include "components/collision/collision.h"

class Physics : public Component {
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
        double max_air_dive_speed = 0.0;
        double air_dive_multiplier = 0.0;

        // Other
        double jump_impulse = 0.0;
        double wall_slide_friction = 0.0;
        double wall_jump_horizontal_impulse = 0.0;
        double wall_jump_vertical_impulse = 0.0;
        double dash_speed = 0.0;
        double dash_friction = 0.0;
        double air_dive_impulse = 0.0;
    };

    Physics(std::weak_ptr<StatefulEntity> statefulEntity,
            std::weak_ptr<MovableEntity> movableEntity,
            std::weak_ptr<AnimatedEntity> animatedEntity,
            std::weak_ptr<Actions> actions);

    void update() override;

    void setPhysicsConstants(Constants constants);

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

  private:
    std::weak_ptr<StatefulEntity> statefulEntity_;
    std::weak_ptr<MovableEntity> movableEntity_;
    std::weak_ptr<AnimatedEntity> animatedEntity_;
    std::weak_ptr<Actions> actions_;

    void resetJumps();

    // TODO Store in some separate component
    int jumps_left_ = 0;

    Constants constants_;
};
