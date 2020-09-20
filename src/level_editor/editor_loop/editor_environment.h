#pragma once

#include <memory>

#include "base_entity.h"
#include "mouse.h"
#include "command.h"
#include "menu.h"
#include "editor_loop/common.h"

#include <SFML/Graphics.hpp>

class EditorEnvironment {
  public:

    enum EditorEntities {
        LAYER_HUD_TEXT,
        MOUSE_HUD_TEXT,
        CURRENT_ENTITY_HUD_TEXT,

        MAX_ENTITIES,
    };

    static constexpr float VIEW_POS_X = 500.0;
    static constexpr float VIEW_POS_Y = 500.0;
    static constexpr float VIEW_SIZE = 1000.0;

    static std::shared_ptr<EditorEnvironment> create_environment(sf::RenderWindow& window);

    RenderableEntity::Layer change_layer(bool towards_screen);

    // Data
    sf::Event event;
    std::shared_ptr<Mouse> mouse;

    std::shared_ptr<History> history = std::make_shared<History>();
    std::shared_ptr<Operation> current_operation = std::make_shared<Operation>();

    std::shared_ptr<Menu> menu;

    Command::Commands current_command = Command::Commands::NONE;
    std::shared_ptr<Command> command = nullptr;

    RenderableEntity::Layer current_layer = RenderableEntity::Layer::MAIN;

    std::shared_ptr<BaseEntity> current_entity;
    std::array<std::shared_ptr<BaseEntity>, EditorEntities::MAX_ENTITIES> editor_entities;

    float view_pos_x;
    float view_pos_y;
    float view_size;

  private:
    EditorEnvironment(sf::RenderWindow& window);

};
