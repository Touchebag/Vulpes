#pragma once

#include "i_render.h"
#include "shader_handle.h"

class EditorRender;

class Render : public IRender {
  friend EditorRender;

  public:
    Render();

    void render(sf::RenderTarget& window, double frame_fraction) override;

    void addEntity(std::weak_ptr<Rendering> entity) override;
    void setPlayer(std::weak_ptr<Rendering> entity) override;
    void setBackground(std::string background) override;

    void setWindowSize(sf::RenderWindow& window, int width, int height) override;

    void clearShaders() override;

    void addShader(std::shared_ptr<ShaderHandle> shader, int layer) override;
    void addGlobalShader(std::shared_ptr<ShaderHandle> shader) override;

  private:
    void drawHud(sf::RenderTarget& window);
    void drawBackground(sf::RenderTarget& window);
    void drawPlayer(sf::RenderTarget& window, double frame_fraction);

    void renderLayerWithPostProcessing(sf::RenderTarget& window, int layer, double frame_fraction);

    // Layers
    struct RenderLayer {
        std::vector<std::weak_ptr<Rendering>> renderables;
        std::vector<std::weak_ptr<ShaderHandle>> shaders;
        double parallax_multiplier;
    };

    sf::Texture background_;
    std::vector<RenderLayer> background_layers_;

    RenderLayer main_layer_;
    std::weak_ptr<Rendering> player_;

    std::vector<RenderLayer> foreground_layers_;
    RenderLayer hud_layer_;

    // Helper functions

    std::vector<std::weak_ptr<Rendering>>& getLayerRenderables(int layer);
    RenderLayer& getLayer(int layer);

    void renderLayer(sf::RenderTarget& target, double frame_fraction, int layer);
    sf::RenderTexture renderToTexture(std::pair<int, int> size, double frame_fraction, int layer);

    bool parallax_enabled_ = true;

    std::vector<std::weak_ptr<ShaderHandle>> global_shaders_;

    // Primary/secondary textures
    std::shared_ptr<sf::RenderTexture> front_render_buffer_;
    std::shared_ptr<sf::RenderTexture> back_render_buffer_;

    std::shared_ptr<sf::RenderTexture> render_texture_;
};
