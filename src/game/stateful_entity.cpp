#include "stateful_entity.h"

#include "file.h"
#include "json.hpp"

StatefulEntity::StatefulEntity() {
    // TODO Take as argument
    std::string filepath = "assets/player_state.json";

    auto j = file::loadJson(filepath);

    // TODO Error handling
    if (j) {
        for (auto state : j.value()) {
            state_list_.insert(std::make_pair(state["id"].get<int>(), State::loadStateFromJson(state)));
        }
    }

    // TODO Error handling
    current_state_ = &state_list_.at(1);
}

void StatefulEntity::updateState() {
    current_state_->update();
}

void StatefulEntity::incomingEvent(state::Event event) {
    std::optional<int> new_state = current_state_->incomingEvent(event);

    if (new_state) {
        current_state_ = &state_list_.at(new_state.value());
        frame_counter_ = current_state_->frame_timer_;
    }
}

std::string StatefulEntity::getCurrentSpriteName() {
    return current_state_->getCurrentSpriteName();
}

bool StatefulEntity::getProperty(state::Property property) {
    switch(property) {
        case state::Property::TOUCHING_GROUND:
            return current_state_->touching_ground_;
        case state::Property::TOUCHING_LEFT_WALL:
            return current_state_->touching_left_wall_;
        case state::Property::TOUCHING_RIGHT_WALL:
            return current_state_->touching_right_wall_;
        case state::Property::MOVEMENT_LOCKED:
            return current_state_->movement_locked_;
        default:
            throw std::runtime_error("Unknown property");
            return false;
    }
}
