#include "stateful.h"

#include "utils/file.h"
#include "json.hpp"

StatefulEntity::StatefulEntity(std::weak_ptr<AnimatedEntity> animatedEntity) :
    animatedEntity_(animatedEntity) {
}

void StatefulEntity::update() {
    if (frame_counter_-- == 0) {
        incomingEvent(state::Event::FRAME_TIMEOUT);
    }
}

void StatefulEntity::loadFromJson(nlohmann::json j) {
    state_file_path_ = j["states"].get<std::string>();

    loadStates(state_file_path_);
}

std::optional<nlohmann::json> StatefulEntity::outputToJson() {
    nlohmann::json j;

    j["states"] = state_file_path_;

    return {j};
}

void StatefulEntity::loadStates(std::string file_path) {
    auto j = File::loadStates(file_path);

    // TODO Error handling
    if (j) {
        for (auto state = j->begin(); state != j->end(); ++state) {
            state_list_.insert(std::make_pair(state.key(),
                               std::make_shared<State>(State::loadStateFromJson(state.value()))));
        }
    }

    // TODO Error handling
    current_state_ = state_list_.at("main");
}

void StatefulEntity::incomingEvent(state::Event event) {
    std::optional<std::string> new_state = current_state_->incomingEvent(event);

    if (new_state) {
        // TODO Error handling
        current_state_ = state_list_.find(new_state.value())->second;
        frame_counter_ = current_state_->properties_.frame_timer_;
        if (auto tmp = animatedEntity_.lock()) {
            tmp->setFrameList(current_state_->properties_.frame_names_);
        }
    }
}

const state::Properties& StatefulEntity::getStateProperties() {
    return current_state_->properties_;
}
