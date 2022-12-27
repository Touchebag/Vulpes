#pragma once

#include "menu_component.h"

class MenuAI : public MenuComponent {
  public:
    void drawMenu(std::shared_ptr<EditorEnvironment> editor_env) override;
};
