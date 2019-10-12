#pragma once

#include <string>
#include <array>
#include <memory>
#include <vector>

#include "base_entity.h"
#include "command.h"

class MenuEntry {
  public:
    MenuEntry(const std::string& text, std::optional<Command::Commands> action = std::nullopt);

    std::string getText();
    void setText(const std::string& text);

    void setColor(sf::Color color);
    sf::Color getColor();

    std::shared_ptr<MenuEntry> selectOption(int option);
    void addEntry(std::shared_ptr<MenuEntry> entry);

    std::vector<std::shared_ptr<BaseEntity>> draw();

    std::optional<Command::Commands> getAction();

  private:
    std::string text_;
    sf::Color color_ = sf::Color::Green;

    std::optional<Command::Commands> action_;

    std::vector<std::shared_ptr<MenuEntry>> entries_;
};
