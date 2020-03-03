#include "stateful.h"

#include "utils/file.h"
#include "json.hpp"

StatefulEntity::StatefulEntity(std::weak_ptr<AnimatedEntity> animatedEntity) :
    animatedEntity_(animatedEntity) {
}

void StatefulEntity::update() {
    if (frame_counter_-- == 0) {
        incomingEvent(state_utils::Event::FRAME_TIMEOUT);
    }
}

void StatefulEntity::reloadFromJson(nlohmann::json j) {
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
        state_handler_.reloadFromJson(j.value());
    }
}

void StatefulEntity::incomingEvent(state_utils::Event event) {
    auto new_state = state_handler_.incomingEvent(event);

    if (auto ns = new_state.lock()) {
        frame_counter_ = ns->getData().frame_timer_;
        if (auto tmp = animatedEntity_.lock()) {
            tmp->setFrameList(ns->getData().frame_names_);
        }
    }
}

const state_utils::Properties& StatefulEntity::getStateProperties() {
    return state_handler_.getStateData();
}
