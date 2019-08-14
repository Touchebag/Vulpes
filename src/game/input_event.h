#pragma once

#include <unordered_map>
#include <memory>
#include <SFML/Window.hpp>

#include "attributes/actions.h"

class Input {
  public:
    static Input& getInstance();

    void keyEvent(sf::Keyboard::Key key, bool pressed);

    void setKeyboardMap(std::unordered_map<sf::Keyboard::Key, Actions::Action> key_map);

    void setActionsInstance(std::weak_ptr<Actions> actions);

  private:
    // The instance of actions where to send keyboard inputs
    std::weak_ptr<Actions> actions_instance_;

    // Input mappings from physical keys to logical buttons
    std::unordered_map<sf::Keyboard::Key, Actions::Action> key_map_;

    Input() = default;
};
