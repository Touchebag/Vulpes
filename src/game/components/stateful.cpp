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
        state_handler_.loadFromJson(j.value());
    }
}

void StatefulEntity::incomingEvent(state::Event event) {
    auto new_state = state_handler_.incomingEvent(event);

    if (auto ns = new_state.lock()) {
        frame_counter_ = ns->properties_.frame_timer_;
        if (auto tmp = animatedEntity_.lock()) {
            tmp->setFrameList(ns->properties_.frame_names_);
        }
    }
}

const state::Properties& StatefulEntity::getStateProperties() {
    return state_handler_.getStateData();
}
