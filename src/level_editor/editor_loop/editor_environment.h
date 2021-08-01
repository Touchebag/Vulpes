#pragma once

#include <memory>

#include "base_entity.h"
#include "mouse.h"
#include "editor_loop/common.h"
#include "command.h"

#include <SFML/Graphics.hpp>

class EditorEnvironment {
  public:
    static constexpr float VIEW_POS_X = 500.0;
    static constexpr float VIEW_POS_Y = 500.0;
    static constexpr float VIEW_SIZE = 1000.0;

    static std::shared_ptr<EditorEnvironment> create_environment(sf::RenderWindow& window);

    int change_layer(bool towards_screen);

    // Data
    sf::Event event;
    std::shared_ptr<Mouse> mouse;

    Command::Commands current_command = Command::Commands::NONE;
    std::shared_ptr<Command> command = nullptr;

    int current_layer = 0;

    std::shared_ptr<BaseEntity> current_entity;

    float view_pos_x;
    float view_pos_y;
    float view_size;

    nlohmann::json entity_clipboard;

  private:
    EditorEnvironment(sf::RenderWindow& window);
};
