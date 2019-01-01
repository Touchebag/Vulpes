#include "stateful_entity.h"

StatefulEntity::StatefulEntity() {
    state::InitParams init1;
    init1.next_states = {{state::Event::TOUCHING_WALL_LEFT, 2}, {state::Event::TOUCHING_WALL_RIGHT, 3}, {state::Event::JUMP, 4}};

    state::InitParams init2;
    init2.movement_locked = true;
    init2.touching_left_wall = true;
    init2.touching_ground = false;
    init2.next_states = {{state::Event::TOUCHING_FLOOR, 1}, {state::Event::TOUCHING_WALL_LEFT, 2}, {state::Event::JUMP, 4}};

    state::InitParams init3;
    init3.movement_locked = true;
    init3.touching_right_wall = true;
    init3.touching_ground = false;
    init3.next_states = {{state::Event::TOUCHING_FLOOR, 1}, {state::Event::TOUCHING_WALL_RIGHT, 3}, {state::Event::JUMP, 4}};

    state::InitParams init4;
    init4.movement_locked = true;
    init4.touching_ground = false;
    init4.next_states = {{state::Event::TOUCHING_FLOOR, 1}, {state::Event::TOUCHING_WALL_LEFT, 2}, {state::Event::TOUCHING_WALL_RIGHT, 3}};

    state_list_.insert({{1, init1}, {2, init2}, {3, init3}, {4, init4}});

    // TODO Error handling
    current_state_ = &state_list_.at(1);
}

void StatefulEntity::incomingEvent(state::Event event) {
    std::optional<int> new_state = current_state_->incomingEvent(event);

    if (new_state) {
        current_state_ = &state_list_.at(new_state.value());
    }
}

bool StatefulEntity::getProperty(state::Property property) {
    switch(property) {
        case state::Property::TOUCHING_GROUND:
            return current_state_->touching_ground_;
        case state::Property::TOUCHING_LEFT_WALL:
            return current_state_->touching_left_wall_;
        case state::Property::TOUCHING_RIGHT_WALL:
            return current_state_->touching_right_wall_;
        case state::Property::MOVEMENT_LOCKED:
            return current_state_->movement_locked_;
        default:
            throw std::runtime_error("Unknown property");
            return false;
    }
}
