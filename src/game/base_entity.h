#pragma once

#include <json.hpp>

#include "utils/common.h"

#include "attributes/hitbox.h"
#include "attributes/transform.h"
#include "attributes/movable.h"
#include "attributes/renderable.h"
#include "attributes/animated.h"
#include "attributes/stateful.h"
#include "attributes/actions.h"

class BaseEntity {
public:
    virtual void update();

    // TODO Move to constructor?
    void setPosition(int abs_x, int abs_y);
    util::Point getPosition();
    void setHitbox(int width, int height);
    const Hitbox getHitbox();
    Hitbox getAbsHitbox();

    virtual void loadFromJson(nlohmann::json j);
    virtual std::optional<nlohmann::json> outputToJson();

    void setTextureCoords(std::pair<util::Point, util::Point>);

    // TODO Store layer in Transform object?
    // That way Renderable can handle all interaction with Render
    std::weak_ptr<RenderableEntity> getRenderable();

    virtual ~BaseEntity() {};

protected:
    // Attributes
    std::shared_ptr<Transform> trans_ = nullptr;
    std::shared_ptr<Hitbox> hitbox_ = nullptr;
    std::shared_ptr<MovableEntity> movableEntity_ = nullptr;
    std::shared_ptr<RenderableEntity> renderableEntity_ = nullptr;
    std::shared_ptr<AnimatedEntity> animatedEntity_ = nullptr;
    std::shared_ptr<StatefulEntity> statefulEntity_ = nullptr;
    std::shared_ptr<Actions> actions_ = nullptr;
};
