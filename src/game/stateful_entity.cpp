#include "stateful_entity.h"

#include "file.h"
#include "json.hpp"

StatefulEntity::StatefulEntity() {
    // TODO Take as argument
    std::string filepath = "assets/player_state.json";

    auto j = file::loadJson(filepath);

    // TODO Error handling
    if (j) {
        for (auto state = j->begin(); state != j->end(); ++state) {
            state_list_.insert(std::make_pair(state.key(), State::loadStateFromJson(state.value())));
        }
    }

    // TODO Error handling
    current_state_ = &(state_list_.at("main"));
    incomingEvent(state::Event::START);
}

void StatefulEntity::updateState() {
    current_state_->update();
}

void StatefulEntity::incomingEvent(state::Event event) {
    std::optional<std::string> new_state = current_state_->incomingEvent(event);

    if (new_state) {
        current_state_ = &(state_list_.find(new_state.value())->second);
        frame_counter_ = current_state_->properties_.frame_timer_;
    }
}

std::string StatefulEntity::getCurrentSprite() {
    return current_state_->getCurrentSprite();
}

const state::Properties& StatefulEntity::getStateProperties() {
    return current_state_->properties_;
}
