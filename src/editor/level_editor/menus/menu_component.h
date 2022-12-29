#pragma once

#include <imgui.h>
#include <imgui-SFML.h>

#include "level_editor/editor_environment.h"
#include "system/system.h"

class MenuComponent {
  public:
    virtual ~MenuComponent() = default;

    virtual void drawMenu(std::shared_ptr<EditorEnvironment> editor_env) = 0;

    bool is_menu_open_ = false;
};
