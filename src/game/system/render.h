#pragma once

#include "i_render.h"

class Render : public IRender {
  public:
    void render(sf::RenderWindow& window) override;

    void addEntity(std::weak_ptr<RenderableEntity> entity) override;

    void moveView(float x, float y) override;
    void resizeView(float width, float height) override;
    void setView(float x, float y, float width, float height) override;
    sf::View getView() override;

    void renderLayer(sf::RenderWindow& window, RenderableEntity::Layer layer);
    bool parallax_enabled_ = true;

  private:
    void drawHud(sf::RenderWindow& window);

    float view_x_;
    float view_y_;
    float view_width_;
    float view_height_;

    std::array<std::vector<std::weak_ptr<RenderableEntity>>, static_cast<int>(RenderableEntity::Layer::MAX_LAYERS)> layers_;
    std::vector<std::weak_ptr<RenderableEntity>> hud_layer_;
};
