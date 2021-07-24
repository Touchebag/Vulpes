#pragma once

#include <imgui.h>
#include <imgui-SFML.h>

#include "editor_loop/editor_environment.h"

class MenuComponent {
  public:
    virtual ~MenuComponent() = default;

    virtual void drawMenu(std::shared_ptr<EditorEnvironment> editor_env) = 0;

    bool is_menu_open_ = false;
};
