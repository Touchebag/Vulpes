#pragma once

#include "components/rendering/rendering.h"

#include <SFML/Graphics.hpp>

class IRender {
  public:
    virtual void render(sf::RenderWindow& window, float frame_fraction) = 0;

    virtual void addEntity(std::weak_ptr<RenderableEntity> entity) = 0;
    virtual void setPlayer(std::weak_ptr<RenderableEntity> entity) = 0;

    virtual void setWindowSize(sf::RenderWindow& window, int width, int height) = 0;
    virtual void loadLayerShaders(nlohmann::json j) = 0;
};
