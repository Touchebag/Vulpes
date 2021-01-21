#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <SFML/Window.hpp>

#include "components/actions/actions.h"

class Input {
  public:
    static Input& getInstance();

    void update(sf::Window& window);

    void setKeyboardMap(std::unordered_map<sf::Keyboard::Key, std::unordered_set<Actions::Action>> key_map);

    void setActionsInstance(std::weak_ptr<Actions> actions);

  private:
    // The instance of actions where to send keyboard inputs
    std::weak_ptr<Actions> actions_instance_;

    // Input mappings from physical keys to logical buttons
    std::unordered_map<sf::Keyboard::Key, std::unordered_set<Actions::Action>> key_map_;

    Input() = default;
};
