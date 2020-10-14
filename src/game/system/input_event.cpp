#include "system/input_event.h"

#include "utils/log.h"

void Input::keyEvent(sf::Keyboard::Key key, bool pressed) {
    if (auto a_inst = actions_instance_.lock()) {
        auto it = key_map_.find(key);

        if (it != key_map_.end()) {
            for (auto action : it->second) {
                if (pressed) {
                    a_inst->addAction(action);
                } else {
                    a_inst->removeAction(action);
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
