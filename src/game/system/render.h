#pragma once

#include "i_render.h"

class EditorRender;

class Render : public IRender {
  friend EditorRender;

  public:
    void render(sf::RenderWindow& window, float frame_fraction) override;

    void addEntity(std::weak_ptr<RenderableEntity> entity) override;

  private:
    void drawHud(sf::RenderWindow& window);

    std::array<std::vector<std::weak_ptr<RenderableEntity>>, static_cast<int>(RenderableEntity::Layer::MAX_LAYERS)> layers_;
    std::vector<std::weak_ptr<RenderableEntity>> hud_layer_;

    void renderLayer(sf::RenderWindow& window, float frame_fraction, RenderableEntity::Layer layer);
    bool parallax_enabled_ = true;
};
