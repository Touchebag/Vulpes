#pragma once

#include "menu_component.h"

class MenuScripting : public MenuComponent {
  public:
    void drawMenu(std::shared_ptr<EditorEnvironment> editor_env) override;
};
