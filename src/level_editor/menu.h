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

    void addEntry(std::shared_ptr<MenuEntry> entry);

    std::shared_ptr<BaseEntity> current_entity;

    std::shared_ptr<MenuEntry> current_entry;
    std::vector<std::shared_ptr<MenuEntry>> entries_;

    std::vector<std::shared_ptr<BaseEntity>> menu_text_;
};
