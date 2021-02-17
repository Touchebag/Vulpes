#pragma once

#include "system/i_render.h"
#include "system/render.h"
#include "editor_loop/editor_environment.h"

class EditorRender : public IRender {
  public:
    void render(sf::RenderWindow& window, float frame_fraction = 0.0f) override;

    void setEditorEnvironment(std::weak_ptr<EditorEnvironment> editor_env);

    void addEntity(std::weak_ptr<RenderableEntity> entity) override;
    void setPlayer(std::weak_ptr<RenderableEntity> entity) override;

    void setWindowSize(sf::RenderWindow& window, int width, int height) override;

    void renderLayer(sf::RenderTarget& target, int layer);
    void setParallaxEnabled(bool enable);
    bool getParallaxEnabled();

    void toggleHitboxRendering();
    void toggleEntranceRendering();

    void setCameraBox(Camera::CameraBoundingBox camera_box);

  private:
    void drawCameraBoundaries(sf::RenderTarget& target);

    Render render_;

    std::weak_ptr<EditorEnvironment> editor_env_;

    bool render_hitboxes_ = false;
    bool render_entrances_ = false;
};
