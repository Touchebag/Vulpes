#include "state.h"

State::State(state::InitParams ips) :
    movement_locked_(ips.movement_locked),
    frame_timer_(ips.frame_timer),
    next_state_list_(ips.next_states) {
    // TODO check for at least one state
}

std::optional<int> State::incomingEvent(state::Event event) {
    auto next_state = next_state_list_.find(event);

    if (next_state != next_state_list_.end()) {
        return std::optional<int>{ next_state->second };
    } else {
        return std::nullopt;
    }
}

