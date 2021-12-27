#pragma once

#include "system/i_render.h"
#include "system/render.h"
#include "editor_loop/editor_environment.h"
#include "components/collision/collideable.h"

class EditorRender : public IRender {
  public:
    void render(sf::RenderTarget& window, double frame_fraction = 0.0) override;

    void setEditorEnvironment(std::weak_ptr<EditorEnvironment> editor_env);

    void addEntity(std::weak_ptr<Rendering> entity) override;
    void setBackground(std::string background) override;
    void setPlayer(std::weak_ptr<Rendering> entity) override;

    void setWindowSize(sf::RenderWindow& window, int width, int height) override;

    void clearShaders() override;

    void addShader(std::shared_ptr<ShaderHandle> shader, int layer) override;
    void addGlobalShader(std::shared_ptr<ShaderHandle> shader) override;

    void renderLayer(sf::RenderTarget& target, int layer);
    void setParallaxEnabled(bool enable);
    bool getParallaxEnabled();

    void setCameraBox(Camera::CameraBoundingBox camera_box);

    // Used for the collision editor menu
    void renderCollideable(std::shared_ptr<Collideable> coll);

    bool render_hitboxes_ = false;
    bool render_entrances_ = false;

  private:
    void drawCameraBoundaries(sf::RenderTarget& target);

    Render render_;

    std::weak_ptr<EditorEnvironment> editor_env_;

    std::shared_ptr<Collideable> collideable_render_;
};
