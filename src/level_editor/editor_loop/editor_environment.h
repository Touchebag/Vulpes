#pragma once

#include <memory>

#include "base_entity.h"
#include "mouse.h"
#include "editor_loop/common.h"
#include "command.h"

#include <SFML/Graphics.hpp>

class MenuActions;
class MenuAI;
class MenuAnimation;
class MenuCollision;
class MenuDamageable;
class MenuDeath;
class MenuMovement;
class MenuPhysics;
class MenuRendering;
class MenuStateful;
class MenuSubentity;
class MenuTransform;

struct MenuList {
    std::shared_ptr<MenuActions> Actions;
    std::shared_ptr<MenuAI> AI;
    std::shared_ptr<MenuAnimation> Animation;
    std::shared_ptr<MenuCollision> Collision;
    std::shared_ptr<MenuDamageable> Damageable;
    std::shared_ptr<MenuDeath> Death;
    std::shared_ptr<MenuMovement> Movement;
    std::shared_ptr<MenuPhysics> Physics;
    std::shared_ptr<MenuRendering> Rendering;
    std::shared_ptr<MenuStateful> Stateful;
    std::shared_ptr<MenuSubentity> Subentity;
    std::shared_ptr<MenuTransform> Transform;
};

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

    MenuList menus;

  private:
    EditorEnvironment(sf::RenderWindow& window);
};
