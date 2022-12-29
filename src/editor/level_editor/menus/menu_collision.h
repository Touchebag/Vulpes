#pragma once

#include "menu_component.h"

class MenuCollision : public MenuComponent {
  public:
    void drawMenu(std::shared_ptr<EditorEnvironment> editor_env) override;

  private:
    void drawSize(std::shared_ptr<EditorEnvironment> editor_env, std::shared_ptr<Collision> coll);

    bool match_render_size_ = false;
};
