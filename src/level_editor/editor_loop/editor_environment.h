#pragma once

#include <memory>

#include "base_entity.h"
#include "mouse.h"
#include "command.h"
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

    EditorEnvironment(sf::RenderWindow& window);

    sf::Event event;
    std::shared_ptr<Mouse> mouse;

    std::shared_ptr<History> history = std::make_shared<History>();
    std::shared_ptr<Operation> current_operation = std::make_shared<Operation>();

    Command::Commands current_action;
    Command command;

    RenderableEntity::Layer current_layer;

    std::shared_ptr<BaseEntity> current_entity;
    std::array<std::shared_ptr<BaseEntity>, EditorEntities::MAX_ENTITIES> editor_entities;
};
