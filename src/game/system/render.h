#pragma once

#include "base_entity.h"

#include <SFML/Graphics.hpp>

class Render {
  public:
    void render(sf::RenderWindow& window);

    void addEntity(std::weak_ptr<RenderableEntity> entity);

    void moveView(float x, float y);
    void resizeView(float width, float height);
    void setView(float x, float y, float width, float height);
    sf::View getView();

    static Render& getInstance();

    // Needed for Level Editor
    void renderLayer(sf::RenderWindow& window, RenderableEntity::Layer layer);
    bool parallax_enabled_ = true;

  private:
    void drawHud(sf::RenderWindow& window);

    float view_x_;
    float view_y_;
    float view_width_;
    float view_height_;

    Render() = default;

    std::array<std::vector<std::weak_ptr<RenderableEntity>>, static_cast<int>(RenderableEntity::Layer::MAX_LAYERS)> layers_;
    std::vector<std::weak_ptr<RenderableEntity>> hud_layer_;
};
