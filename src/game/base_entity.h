#pragma once

#include <json.hpp>

#include "transform.h"
#include "hitbox.h"
#include "utils/common.h"

#include "attributes/movable.h"
#include "attributes/renderable.h"

class BaseEntity {
public:
    virtual void update();

    // TODO Move to constructor?
    void setPosition(int abs_x, int abs_y);
    sf::Vector2i getPosition();
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
    std::shared_ptr<Transform> trans_;
    std::shared_ptr<Hitbox> hitbox_;

    // Attributes
    std::shared_ptr<MovableEntity> movableEntity_ = nullptr;
    std::shared_ptr<RenderableEntity> renderableEntity_ = nullptr;
};
