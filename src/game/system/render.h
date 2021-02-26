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
    struct RenderLayer {
        std::vector<std::weak_ptr<RenderableEntity>> renderables;
        std::shared_ptr<sf::Shader> shader;
        float parallax_multiplier;
    };

    std::weak_ptr<RenderableEntity> background_;
    std::vector<RenderLayer> background_layers_;

    RenderLayer main_layer_;
    std::weak_ptr<RenderableEntity> player_;

    std::vector<RenderLayer> foreground_layers_;
    RenderLayer hud_layer_;

    // Helper functions

    std::vector<std::weak_ptr<RenderableEntity>>& getLayerRenderables(int layer);
    RenderLayer& getLayer(int layer);

    void renderLayer(sf::RenderTarget& target, float frame_fraction, int layer);
    sf::RenderTexture renderToTexture(std::pair<int, int> size, float frame_fraction, int layer);

    bool parallax_enabled_ = true;

    sf::Sprite render_layer_sprite_;
    sf::RenderTexture render_texture_;
};
