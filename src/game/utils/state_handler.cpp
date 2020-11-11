#include "utils/state_handler.h"

#include "ai/logic_operators/logic_operator.h"
#include "components/actions/actions.h"

template <class T>
void StateHandler<T>::resetState() {
    // TODO Error handling
    current_state_ = state_list_.at("main");
}

template <class T>
void StateHandler<T>::reloadFromJson(const nlohmann::json& j) {
    for (auto state = j.begin(); state != j.end(); ++state) {
        state_list_.insert(std::make_pair(state.key(),
                    std::make_shared<State<T>>(
                        State<T>::loadStateFromJson(state.value()))));
    }

    resetState();
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


template class StateHandler<state_utils::EntityContent>;
template class StateHandler<std::vector<std::pair<std::shared_ptr<const ai::condition::LogicalOperator>, Actions::Action>>>;
