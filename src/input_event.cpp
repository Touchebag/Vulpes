#include "input_event.h"

#include "log.h"

void Input::update() {
    for (auto &it : current_buttons_) {
        it.second = false;
    }
}

void Input::keyEvent(sf::Keyboard::Key key, bool pressed) {
    auto it = key_map_.find(key);

    if (it != key_map_.end()) {
        if (pressed) {
            current_buttons_.insert_or_assign(it->second, true);
        } else {
            current_buttons_.erase(it->second);
        }
    }
}

bool Input::isButtonHeld(input::button button) {
    return (current_buttons_.find(button) != current_buttons_.end());
}

bool Input::isButtonPressed(input::button button) {
    auto it = current_buttons_.find(button);
    return (it != current_buttons_.end() && it->second);
}

void Input::setKeyboardMap(std::unordered_map<sf::Keyboard::Key, input::button> key_map) {
    key_map_ = key_map;
}

Input& Input::getInstance() {
    static Input instance;
    return instance;
}
