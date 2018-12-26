#pragma once

#include <SFML/Graphics.hpp>
#include <json.hpp>

#include "transform.h"
#include "hitbox.h"
#include "util.h"

class BaseEntity {
public:
    BaseEntity();

    virtual void update();

    // TODO Move to constructor?
    void setPosiition(util::X abs_x, util::Y abs_y);
    void setHitbox(float left, float right, float top, float bottom);
    Hitbox getAbsHitbox();

    void loadFromJson(nlohmann::json j);
    void render(sf::RenderWindow& window);

protected:
    Transform trans_;
    Hitbox hitbox_;

private:
    sf::Texture texture_;
    sf::Sprite sprite_;
};
