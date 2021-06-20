#pragma once

#include "utils/common.h"
#include "components/transform.h"
#include "components/collision/collision.h"

#include <utility>
#include <memory>

class Movement : public Component {
  public:
    struct MovementAttributes {
        bool on_ground = false;
        bool touching_wall = false;
        bool falling = false;
        bool on_slope = false;
    };

    Movement(std::weak_ptr<ComponentStore> components);

    void move(double velX, double velY);

    void update() override;

    static std::shared_ptr<Movement> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

    void setVelocity(double x, double y);
    double getVelX();
    double getVelY();

    const MovementAttributes& getMovementAttributes();

    bool isFacingRight();
    void setFacingRight(bool facing_right);

  private:
    std::pair<double, double> getMaximumMovement(double velX, double velY);

    double velx_ = 0.0;
    double vely_ = 0.0;

    MovementAttributes move_attr_;

    bool facing_right_ = true;
};
