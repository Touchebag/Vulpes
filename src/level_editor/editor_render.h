#pragma once

#include "system/i_render.h"
#include "system/render.h"
#include "editor_loop/editor_environment.h"

class EditorRender : public IRender {
  public:
    void render(sf::RenderWindow& window) override;

    void setEditorEnvironment(std::weak_ptr<EditorEnvironment> editor_env);

    void addEntity(std::weak_ptr<RenderableEntity> entity) override;

    void moveView(float x, float y) override;
    void resizeView(float width, float height) override;
    void setView(float x, float y, float width, float height) override;
    sf::View getView() override;

    void renderLayer(sf::RenderWindow& window, RenderableEntity::Layer layer);
    void setParallaxEnabled(bool enable);
    bool getParallaxEnabled();

    void toggleHitboxRendering();

    void setCameraBox(CameraBox camera_box) override;

  private:
    void drawCameraBoundaries(sf::RenderWindow& window);

    Render render_;

    std::weak_ptr<EditorEnvironment> editor_env_;

    bool render_hitboxes_ = false;
};
