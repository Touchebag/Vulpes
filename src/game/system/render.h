#pragma once

#include "i_render.h"

class EditorRender;

class Render : public IRender {
  friend EditorRender;

  public:
    void render(sf::RenderWindow& window) override;

    void addEntity(std::weak_ptr<RenderableEntity> entity) override;

    void moveView(float x, float y) override;
    void resizeView(float width, float height) override;
    void setView(float x, float y, float width, float height) override;
    sf::View getView() override;

    void setCameraBox(CameraBox camera_box) override;

  private:
    void drawHud(sf::RenderWindow& window);

    float view_x_;
    float view_y_;
    float view_width_;
    float view_height_;

    CameraBox camera_box_;

    std::array<std::vector<std::weak_ptr<RenderableEntity>>, static_cast<int>(RenderableEntity::Layer::MAX_LAYERS)> layers_;
    std::vector<std::weak_ptr<RenderableEntity>> hud_layer_;

    void renderLayer(sf::RenderWindow& window, RenderableEntity::Layer layer);
    bool parallax_enabled_ = true;
};
