#pragma once

#include "menu_component.h"

class MenuTransform : public MenuComponent {
  public:
    void drawMenu(std::shared_ptr<EditorEnvironment> editor_env) override;
};
