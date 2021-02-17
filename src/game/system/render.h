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

    void setWindowSize(sf::RenderWindow& window, int width, int height) override;

  private:
    void drawHud(sf::RenderWindow& window);
    void drawPlayer(sf::RenderWindow& window, float frame_fraction);

    void renderLayerWithPostProcessing(sf::RenderWindow& window, int layer, float frame_fraction);

    // Layers
    std::weak_ptr<RenderableEntity> background_;
    std::vector<std::vector<std::weak_ptr<RenderableEntity>>> background_layers_;

    std::vector<std::weak_ptr<RenderableEntity>> main_layer_;
    std::weak_ptr<RenderableEntity> player_;

    std::vector<std::vector<std::weak_ptr<RenderableEntity>>> foreground_layers_;
    std::vector<std::weak_ptr<RenderableEntity>> hud_layer_;

    // Helper functions

    std::vector<std::weak_ptr<RenderableEntity>>& getLayer(int layer);
    void renderLayer(sf::RenderTarget& target, float frame_fraction, int layer);
    sf::RenderTexture renderToTexture(std::pair<int, int> size, float frame_fraction, int layer);

    bool parallax_enabled_ = true;

    sf::Sprite render_layer_sprite;
    sf::RenderTexture render_texture_;
};
