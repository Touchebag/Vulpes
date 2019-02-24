#include "state.h"

#include "file.h"

State::State(state::InitParams ips) :
    // TODO Store actual InitParams struct instead
    movement_locked_(ips.movement_locked),
    touching_ground_(ips.touching_ground),
    touching_right_wall_(ips.touching_right_wall),
    touching_left_wall_(ips.touching_left_wall),
    can_jump_(ips.can_jump),
    frame_timer_(ips.frame_timer),
    next_state_list_(ips.next_states),
    frame_names_(ips.frame_names) {
    // TODO check for at least one following state
}

void State::update() {
    if (++current_frame_ >= frame_names_.size()) {
        current_frame_ = 0;
    }
}

State State::loadStateFromJson(nlohmann::json j) {
    state::InitParams ips;

    // If an option is not found use default
    if (j.find("movement_locked") != j.end()) {
        ips.movement_locked = j["movement_locked"].get<bool>();
    }
    if (j.find("touching_ground") != j.end()) {
        ips.touching_ground = j["touching_ground"].get<bool>();
    }
    if (j.find("touching_right_wall") != j.end()) {
        ips.touching_right_wall = j["touching_right_wall"].get<bool>();
    }
    if (j.find("touching_left_wall") != j.end()) {
        ips.touching_left_wall = j["touching_left_wall"].get<bool>();
    }
    if (j.find("can_jump") != j.end()) {
        ips.can_jump = j["can_jump"].get<bool>();
    }
    if (j.find("frame_timer") != j.end()) {
        ips.frame_timer = j["frame_timer"].get<unsigned int>();
    }

    {
        // Exceptions are not recoverable
        // Let propagate and crash
        nlohmann::json next_state_array = j["next_states"];

        for (auto it : next_state_array) {
            ips.next_states.insert(std::make_pair(state::Event(it["event"].get<int>()), it["state"].get<std::string>()));
        }

        nlohmann::json frame_names_array = j["frame_names"];

        for (auto it : frame_names_array) {
            ips.frame_names.push_back(it.get<std::string>());
        }
    }

    return State(ips);
}

std::optional<std::string> State::incomingEvent(state::Event event) {
    auto next_state = next_state_list_.find(event);

    if (next_state != next_state_list_.end()) {
        // TODO Error handling
        return std::optional<std::string>{ next_state->second };
    } else {
        return std::nullopt;
    }
}

std::string State::getCurrentSprite() {
    return frame_names_.at(current_frame_);
}
