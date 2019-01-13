#pragma once

#include <unordered_map>
#include <SFML/Window.hpp>

#include "utils/input.h"

class Input {
  public:
    static Input& getInstance();

    void update();

    void keyEvent(sf::Keyboard::Key key, bool pressed);

    // Check if the button/key for a specific action is held down.
    bool isButtonHeld(input::button button);
    // Same as above but will only trigger on first frame (key press)
    bool isButtonPressed(input::button button);

    void setKeyboardMap(std::unordered_map<sf::Keyboard::Key, input::button> key_map);

  private:
    // Buttons currently pressed will be stored in this.
    // The boolean value will only be true for the first frame each button is pressed
    std::unordered_map<input::button, bool> current_buttons_;

    // Input mappings from physical keys to logical buttons
    std::unordered_map<sf::Keyboard::Key, input::button> key_map_;

    Input() = default;
};
