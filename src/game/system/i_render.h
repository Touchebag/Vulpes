#pragma once

#include "components/rendering/rendering.h"

#include <SFML/Graphics.hpp>

class IRender {
  public:
    virtual void render(sf::RenderWindow& window) = 0;

    virtual void addEntity(std::weak_ptr<RenderableEntity> entity) = 0;

    virtual void moveView(float x, float y) = 0;
    virtual void resizeView(float width, float height) = 0;
    virtual void setView(float x, float y, float width, float height) = 0;
    virtual sf::View getView() = 0;
};