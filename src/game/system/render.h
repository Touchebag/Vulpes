#pragma once

#include "i_render.h"

class EditorRender;

class Render : public IRender {
  friend EditorRender;

  public:
    Render();

    void render(sf::RenderWindow& window, float frame_fraction) override;

    void addEntity(std::weak_ptr<RenderableEntity> entity) override;
    void setPlayer(std::weak_ptr<RenderableEntity> entity) override;

  private:
    void drawHud(sf::RenderWindow& window);

    // Layers
    std::weak_ptr<RenderableEntity> background_;
    std::vector<std::vector<std::weak_ptr<RenderableEntity>>> background_layers_;

    std::vector<std::weak_ptr<RenderableEntity>> main_layer_;
    std::weak_ptr<RenderableEntity> player_;

    std::vector<std::vector<std::weak_ptr<RenderableEntity>>> foreground_layers_;
    std::vector<std::weak_ptr<RenderableEntity>> hud_layer_;

    // Helper functions

    std::vector<std::weak_ptr<RenderableEntity>>& getLayer(int layer);
    void renderLayer(sf::RenderWindow& window, float frame_fraction, int layer);
    bool parallax_enabled_ = true;
};
