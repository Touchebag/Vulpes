#pragma once

#include "system/i_render.h"
#include "system/render.h"

class EditorRender : public IRender {
  public:
    void render(sf::RenderWindow& window) override;

    void addEntity(std::weak_ptr<RenderableEntity> entity) override;

    void moveView(float x, float y) override;
    void resizeView(float width, float height) override;
    void setView(float x, float y, float width, float height) override;
    sf::View getView() override;

    void renderLayer(sf::RenderWindow& window, RenderableEntity::Layer layer);
    void setParallaxEnabled(bool enable);
    bool getParallaxEnabled();

  private:
    Render render_;
};
