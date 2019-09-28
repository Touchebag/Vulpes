#pragma once

#include <string>
#include <array>
#include <memory>
#include <vector>

class MenuEntry {
  public:
    MenuEntry(const std::string& text);

    std::string getText();
    void setText(const std::string& text);

    void selectOption(int option);
    void addEntry(std::weak_ptr<MenuEntry> entry);

  private:
    std::string text_;

    std::vector<std::weak_ptr<MenuEntry>> entries_;
};
