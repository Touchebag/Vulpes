#pragma once

#include "base_entity.h"
#include "world.h"

#include <SFML/Graphics.hpp>

class Render {
  public:
    void render(sf::RenderWindow& window);

    void addEntity(std::weak_ptr<RenderableEntity> entity, World::Layer layer);
    void removeEntity(std::weak_ptr<RenderableEntity> entity, World::Layer layer);

    void setView(float x, float y, float width, float height);
    sf::View getView();

    static Render& getInstance();

    // Needed for Level Editor
    // TODO Delete renderableEntity in history instead and let Render weak_ptr expire
    void renderLayer(sf::RenderWindow& window, World::Layer layer);
    bool parallax_enabled_ = true;

  private:
    float view_x_;
    float view_y_;
    float view_width_;
    float view_height_;

    Render() = default;

    std::array<std::vector<std::weak_ptr<RenderableEntity>>, static_cast<int>(World::Layer::MAX_LAYERS)> layers_;
};
