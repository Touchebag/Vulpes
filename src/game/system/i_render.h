#pragma once

#include "components/rendering/rendering.h"

#include <SFML/Graphics.hpp>

class IRender {
  public:
    virtual void render(sf::RenderWindow& window) = 0;

    virtual void addEntity(std::weak_ptr<RenderableEntity> entity) = 0;
};
