#include "stateful_entity.h"

StatefulEntity::StatefulEntity() {
    state::InitParams init1;
    init1.next_states = {{state::Event::TOUCHING_WALL_LEFT, 2}, {state::Event::TOUCHING_WALL_RIGHT, 3}};

    state::InitParams init2;
    init2.movement_locked = true;
    init2.next_states = {{state::Event::TOUCHING_FLOOR, 1}, {state::Event::TOUCHING_WALL_LEFT, 2}};

    state::InitParams init3;
    init3.movement_locked = true;
    init3.next_states = {{state::Event::TOUCHING_FLOOR, 1}, {state::Event::TOUCHING_WALL_RIGHT, 3}};

    state_list_.insert({{1, init1}, {2, init2}, {3, init3}});

    // TODO Error handling
    current_state_ = &state_list_.at(1);
}

void StatefulEntity::incomingEvent(state::Event event) {
    std::optional<int> new_state = current_state_->incomingEvent(event);

    if (new_state) {
        current_state_ = &state_list_.at(new_state.value());
    }
}

bool StatefulEntity::isMovementLocked() {
    return current_state_->movement_locked_;
}
