#pragma once

#include "components/rendering/rendering.h"
#include "shader_handle.h"

#include <SFML/Graphics.hpp>

class IRender {
  public:
    virtual ~IRender() = default;

    virtual void render(sf::RenderWindow& window, float frame_fraction) = 0;

    virtual void addEntity(std::weak_ptr<Rendering> entity) = 0;
    virtual void setPlayer(std::weak_ptr<Rendering> entity) = 0;
    virtual void setBackground(std::string background) = 0;

    virtual void setWindowSize(sf::RenderWindow& window, int width, int height) = 0;

    virtual void clearLayerShaders() = 0;
    virtual void addShader(std::shared_ptr<ShaderHandle> shader, int layer) = 0;
};
