#include "state.h"

#include "log.h"

#include "ai/logic_operators/logic_operator.h"
#include "components/actions.h"

template <class T>
State<T>::State(T data) :
    data_(data) {
}

template <class T>
std::optional<std::string> State<T>::incomingEvent(state_utils::Event event) {
    auto next_state = next_states_.find(event);

    if (next_state != next_states_.end()) {
        // TODO Error handling
        return std::optional<std::string>{ next_state->second };
    } else {
        return std::nullopt;
    }
}

template <class T>
const T& State<T>::getData() {
    return data_;
}

template <class T>
void State<T>::loadNextStateListFromJson(nlohmann::json j) {
    for (auto it : j) {

        auto event_name = it["event"].get<std::string>();
        auto event_name_entry = state_utils::string_event_map.find(event_name);

        if (event_name_entry != state_utils::string_event_map.end()) {
            state_utils::Event event = event_name_entry->second;
            next_states_.insert(std::make_pair(event, it["state"].get<std::string>()));
        } else {
            throw std::invalid_argument(std::string("Invalid event: ") + event_name.c_str());
        }
    }
}

template <>
State<state_utils::Properties> State<state_utils::Properties>::loadStateFromJson(nlohmann::json j) {
    state_utils::Properties properties;

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
    if (j.find("dashing") != j.end()) {
        properties.dashing_ = j["dashing"].get<bool>();
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

    // Exceptions are not recoverable
    // Let propagate and crash
    nlohmann::json frame_names_array = j["frame_names"];

    for (auto it : frame_names_array) {
        properties.frame_names_.push_back(it.get<std::string>());
    }

    auto new_state = State(properties);

    new_state.loadNextStateListFromJson(j["next_states"]);

    return new_state;
}

#define AI_CONDITION_TYPE std::pair<std::shared_ptr<const ai::condition::LogicalOperator>, Actions::Action>

template <>
State<std::vector<AI_CONDITION_TYPE>>
State<std::vector<AI_CONDITION_TYPE>>::loadStateFromJson(nlohmann::json j) {
    if (!j.contains("actions")) {
        throw std::invalid_argument("AI, missing actions");
    }

    std::vector<AI_CONDITION_TYPE> ai_behavior;

    for (auto it : j["actions"]) {
        auto condition = ai::condition::LogicalOperator::createFromJson(it["condition"]);
        Actions::Action action = Actions::fromString(it["action"]);
        ai_behavior.push_back(std::make_pair(condition, action));
    }

    auto new_state = State(ai_behavior);

    new_state.loadNextStateListFromJson(j["next_states"]);

    return new_state;
}

template class State<state_utils::Properties>;
template class State<std::vector<AI_CONDITION_TYPE>>;
