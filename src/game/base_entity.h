#pragma once

#include <json.hpp>

#include "utils/common.h"

#include "components/hitbox.h"
#include "components/transform.h"
#include "components/movement.h"
#include "components/rendering/rendering.h"
#include "components/animation.h"
#include "components/stateful.h"
#include "components/actions.h"
#include "components/collision.h"
#include "components/physics.h"

class BaseEntity {
public:
    virtual void update();

    // TODO Move to constructor?
    void setPosition(int abs_x, int abs_y);
    util::Point getPosition();
    void setHitbox(int width, int height);
    const Hitbox getHitbox();

    virtual void loadFromJson(nlohmann::json j);
    virtual std::optional<nlohmann::json> outputToJson();

    void setTextureCoords(std::pair<util::Point, util::Point>);

    virtual ~BaseEntity() {};

    // TODO Components
    // Attributes
    std::shared_ptr<Transform> trans_ = nullptr;
    std::shared_ptr<Hitbox> hitbox_ = nullptr;
    std::shared_ptr<Collision> collision_ = nullptr;
    std::shared_ptr<MovableEntity> movableEntity_ = nullptr;
    std::shared_ptr<RenderableEntity> renderableEntity_ = nullptr;
    std::shared_ptr<AnimatedEntity> animatedEntity_ = nullptr;
    std::shared_ptr<StatefulEntity> statefulEntity_ = nullptr;
    std::shared_ptr<Actions> actions_ = nullptr;
    std::shared_ptr<Physics> physics_ = nullptr;
};
