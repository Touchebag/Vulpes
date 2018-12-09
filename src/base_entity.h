#pragma once

#include <SFML/Graphics.hpp>

#include "transform.h"

class BaseEntity {
public:
    BaseEntity();

    virtual void update();

    // TODO Move to constructor?
    void setPosiition(float abs_x, float abs_y);

    void render(sf::RenderWindow& window);

protected:
    Transform trans_;

private:
    sf::Texture texture_;
    sf::Sprite sprite_;
};
