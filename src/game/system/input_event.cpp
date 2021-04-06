#include "system/input_event.h"

#include "utils/log.h"

void Input::update(sf::Window& window) {
    if (auto a_inst = actions_instance_.lock()) {
        for (auto it : key_map_) {
            if (window.hasFocus() && sf::Keyboard::isKeyPressed(it.first)) {
                for (auto action : it.second) {
                    a_inst->addAction(action);
                }
            }
        }
        if (sf::Joystick::isConnected(0)) {
            for (auto it : button_map_) {
                if (window.hasFocus() && sf::Joystick::isButtonPressed(0, it.first)) {
                    for (auto action : it.second) {
                        a_inst->addAction(action);
                    }
                }
            }

            auto x_axis = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X);
            if (x_axis > 30) {
                a_inst->addAction(Actions::Action::MOVE_RIGHT);
            } else if (x_axis < -30) {
                a_inst->addAction(Actions::Action::MOVE_LEFT);
            }

            auto y_axis = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y);
            if (y_axis < 50) {
                a_inst->addAction(Actions::Action::AIR_DIVE);
            }
        }
    }
}

void Input::setActionsInstance(std::weak_ptr<Actions> actions) {
    actions_instance_ = actions;
}

void Input::setKeyboardMap(std::unordered_map<sf::Keyboard::Key, std::unordered_set<Actions::Action>> key_map) {
    key_map_ = key_map;
}

void Input::setButtonMap(std::unordered_map<int, std::unordered_set<Actions::Action>> button_map) {
    button_map_ = button_map;
}
