#pragma once

#include <SFML/Graphics.hpp>

#include "transform.h"
#include "hitbox.h"

class BaseEntity {
public:
    BaseEntity();

    virtual void update();

    // TODO Move to constructor?
    void setPosiition(float abs_x, float abs_y);
    void setHitbox(float left, float right, float top, float bottom);
    Hitbox getAbsHitbox();

    void render(sf::RenderWindow& window);

protected:
    Transform trans_;
    Hitbox hitbox_;

private:
    sf::Texture texture_;
    sf::Sprite sprite_;
};
