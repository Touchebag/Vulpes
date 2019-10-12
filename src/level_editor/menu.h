#pragma once

#include <vector>
#include <memory>

#include "menu_entry.h"
#include "base_entity.h"

class Menu {
  public:
    Menu(std::shared_ptr<BaseEntity> current_entity);

    std::optional<Command::Commands> selectOption(int option);
  private:
    void draw();

    std::shared_ptr<BaseEntity> current_entity;

    std::shared_ptr<MenuEntry> current_entry;
    std::shared_ptr<MenuEntry> main_entry_;

    std::vector<std::shared_ptr<BaseEntity>> menu_text_;
};
