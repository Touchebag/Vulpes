#include "utils/state_handler.h"

void StateHandler::loadFromJson(nlohmann::json& j) {
    for (auto state = j.begin(); state != j.end(); ++state) {
        state_list_.insert(std::make_pair(state.key(),
                    std::make_shared<State>(State::loadStateFromJson(state.value()))));
    }

    // TODO Error handling
    current_state_ = state_list_.at("main");
}

std::weak_ptr<State> StateHandler::incomingEvent(state::Event event) {
    std::optional<std::string> new_state = current_state_->incomingEvent(event);

    if (new_state) {
        // TODO Error handling
        current_state_ = state_list_.find(new_state.value())->second;
        return current_state_;
    } else {
        return std::weak_ptr<State>();
    }
}

const state::Properties& StateHandler::getStateData() {
    return current_state_->properties_;
}
