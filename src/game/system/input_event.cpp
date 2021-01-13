#include "system/input_event.h"

#include "utils/log.h"

void Input::update() {
    if (auto a_inst = actions_instance_.lock()) {
        for (auto it : key_map_) {
            if (sf::Keyboard::isKeyPressed(it.first)) {
                for (auto action : it.second) {
                    a_inst->addAction(action);
                }
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

Input& Input::getInstance() {
    static Input instance;
    return instance;
}
