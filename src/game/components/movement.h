#pragma once

#include "utils/common.h"
#include "components/transform.h"
#include "components/collision/collision.h"

#include <utility>
#include <memory>

class MovableEntity : public Component {
  public:
    struct MovementAttributes {
        bool on_ground = false;
        bool touching_wall = false;
        bool falling = false;
        bool on_slope = false;
    };

    MovableEntity(std::weak_ptr<ComponentStore> components);

    void move(double velX, double velY);

    void update() override;

    static std::shared_ptr<MovableEntity> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

    double getVelX();
    double getVelY();

    const MovementAttributes& getMovementAttributes();

    bool facing_right_ = true;

  private:
    std::pair<double, double> getMaximumMovement(double velX, double velY);

    double velx_ = 0.0;
    double vely_ = 0.0;

    MovementAttributes move_attr_;
};
