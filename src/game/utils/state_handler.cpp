#include "utils/state_handler.h"

template <>
void StateHandler<state_utils::Properties>::loadFromJson(nlohmann::json& j) {
    for (auto state = j.begin(); state != j.end(); ++state) {
        state_list_.insert(std::make_pair(state.key(),
                    std::make_shared<State<state_utils::Properties>>(
                        State<state_utils::Properties>::loadStateFromJson(state.value()))));
    }

    // TODO Error handling
    current_state_ = state_list_.at("main");
}

template <class T>
std::weak_ptr<State<T>> StateHandler<T>::incomingEvent(state_utils::Event event) {
    std::optional<std::string> new_state = current_state_->incomingEvent(event);

    if (new_state) {
        // TODO Error handling
        current_state_ = state_list_.find(new_state.value())->second;
        return current_state_;
    } else {
        return std::weak_ptr<State<T>>();
    }
}

template <class T>
const T& StateHandler<T>::getStateData() {
    return current_state_->getData();
}
