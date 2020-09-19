#pragma once

#include <vector>
#include <memory>

#include "menu_entry.h"
#include "base_entity.h"

class Menu {
  public:
    Menu(std::weak_ptr<EditorEnvironment> editor_env);

    std::optional<Command::Commands> selectOption(int option);
  private:
    void draw();

    std::weak_ptr<EditorEnvironment> editor_env_;

    std::shared_ptr<MenuEntry> current_entry_;
    std::shared_ptr<MenuEntry> main_entry_;

    std::vector<std::shared_ptr<BaseEntity>> menu_text_;
};
