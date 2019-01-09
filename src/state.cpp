#include "state.h"

#include "file.h"

State::State(state::InitParams ips) :
    movement_locked_(ips.movement_locked),
    touching_ground_(ips.touching_ground),
    touching_right_wall_(ips.touching_right_wall),
    touching_left_wall_(ips.touching_left_wall),
    frame_timer_(ips.frame_timer),
    next_state_list_(ips.next_states) {
    // TODO check for at least one following state
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
    if (j.find("frame_timer") != j.end()) {
        ips.frame_timer = j["frame_timer"].get<unsigned int>();
    }

    {
        // Exceptions are not recoverable
        // Let propagate and crash
        nlohmann::json next_state_array = j["next_states"];

        for (auto it : next_state_array) {
            ips.next_states.insert(std::make_pair(state::Event(it["event"].get<int>()), it["state"].get<int>()));
        }
    }

    return State(ips);
}

std::optional<int> State::incomingEvent(state::Event event) {
    auto next_state = next_state_list_.find(event);

    if (next_state != next_state_list_.end()) {
        // TODO Error handling
        return std::optional<int>{ next_state->second };
    } else {
        return std::nullopt;
    }
}

