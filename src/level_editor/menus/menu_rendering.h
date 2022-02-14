#pragma once

#include "menu_component.h"

class MenuRendering : public MenuComponent {
  public:
    void drawMenu(std::shared_ptr<EditorEnvironment> editor_env) override;

    void setTiling(std::shared_ptr<Rendering> render, int x, int y);
    void setTexture(std::shared_ptr<Rendering> render, std::string texture);

  private:
    void drawSize(std::shared_ptr<EditorEnvironment> editor_env, std::shared_ptr<Rendering>);
    void drawTiling(std::shared_ptr<Rendering> render);
    void drawTextureName(std::shared_ptr<Rendering> render, nlohmann::json j);

    static constexpr int BUFFER_SIZE = 1024;
    char texture_name_buf[BUFFER_SIZE] = {};

    bool match_collision_size_ = false;
};
