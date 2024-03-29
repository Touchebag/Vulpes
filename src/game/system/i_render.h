#pragma once

#include "components/rendering/rendering.h"
#include "shader_handle.h"

#include <SFML/Graphics.hpp>

class IRender {
  public:
    virtual ~IRender() = default;

    virtual void render(sf::RenderTarget& window, double frame_fraction) = 0;

    virtual void addEntity(std::weak_ptr<Rendering> entity) = 0;
    virtual void setPlayer(std::weak_ptr<Rendering> entity) = 0;
    virtual void setBackground(std::string background) = 0;

    virtual void setWindowSize(sf::RenderWindow& window, unsigned int width, unsigned int height) = 0;

    virtual void setBackgroundLayers(std::vector<double> layers) = 0;
    virtual void setForegroundLayers(std::vector<double> layers) = 0;

    virtual void clearLayers() = 0;

    virtual void addShader(std::shared_ptr<ShaderHandle> shader, int layer) = 0;
    virtual void addGlobalShader(std::shared_ptr<ShaderHandle> shader) = 0;
};
