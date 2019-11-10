#include "state.h"

State::State(state::Properties properties) :
    properties_(properties) {
    // TODO check for at least one following state
}

State State::loadStateFromJson(nlohmann::json j) {
    state::Properties properties;

    // If an option is not found use default
    if (j.find("movement_locked_x") != j.end()) {
        properties.movement_locked_x_ = j["movement_locked_x"].get<bool>();
    }
    if (j.find("movement_locked_y") != j.end()) {
        properties.movement_locked_y_ = j["movement_locked_y"].get<bool>();
    }
    if (j.find("touching_ground") != j.end()) {
        properties.touching_ground_ = j["touching_ground"].get<bool>();
    }
    if (j.find("touching_wall") != j.end()) {
        properties.touching_wall_ = j["touching_wall"].get<bool>();
    }
    if (j.find("can_jump") != j.end()) {
        properties.can_jump_ = j["can_jump"].get<bool>();
    }
    if (j.find("can_dash") != j.end()) {
        properties.can_dash_ = j["can_dash"].get<bool>();
    }
    if (j.find("frame_timer") != j.end()) {
        properties.frame_timer_ = j["frame_timer"].get<unsigned int>();
    }

    {
        // Exceptions are not recoverable
        // Let propagate and crash
        nlohmann::json next_state_array = j["next_states"];

        for (auto it : next_state_array) {
            properties.next_states_.insert(std::make_pair(state::Event(it["event"].get<int>()), it["state"].get<std::string>()));
        }

        nlohmann::json frame_names_array = j["frame_names"];

        for (auto it : frame_names_array) {
            properties.frame_names_.push_back(it.get<std::string>());
        }
    }

    return State(properties);
}

std::optional<std::string> State::incomingEvent(state::Event event) {
    auto next_state = properties_.next_states_.find(event);

    if (next_state != properties_.next_states_.end()) {
        // TODO Error handling
        return std::optional<std::string>{ next_state->second };
    } else {
        return std::nullopt;
    }
}

