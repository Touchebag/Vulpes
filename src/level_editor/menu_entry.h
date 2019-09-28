#pragma once

#include <string>
#include <array>
#include <memory>
#include <vector>

#include "base_entity.h"

class MenuEntry {
  public:
    MenuEntry(const std::string& text);

    std::string getText();
    void setText(const std::string& text);

    std::shared_ptr<MenuEntry> selectOption(int option);
    void addEntry(std::weak_ptr<MenuEntry> entry);

    std::vector<std::shared_ptr<BaseEntity>> draw();

  private:
    std::string text_;

    std::vector<std::weak_ptr<MenuEntry>> entries_;
};
