#include "state.h"

#include "log.h"

#include "ai/logic_operators/logic_operator.h"
#include "components/actions/actions.h"

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
State<state_utils::EntityContent> State<state_utils::EntityContent>::loadStateFromJson(nlohmann::json j) {
    state_utils::PhysicsProperties physics_props;
    state_utils::StateProperties state_props;

    nlohmann::json physics_json = j["properties"];

    // If an option is not found use default
    if (physics_json.find("movement_locked_x") != physics_json.end()) {
        physics_props.movement_locked_x_ = physics_json["movement_locked_x"].get<bool>();
    }
    if (physics_json.find("movement_locked_y") != physics_json.end()) {
        physics_props.movement_locked_y_ = physics_json["movement_locked_y"].get<bool>();
    }
    if (physics_json.find("direction_locked") != physics_json.end()) {
        physics_props.direction_locked_ = physics_json["direction_locked"].get<bool>();
    }
    if (physics_json.find("touching_ground") != physics_json.end()) {
        physics_props.touching_ground_ = physics_json["touching_ground"].get<bool>();
    }
    if (physics_json.find("touching_wall") != physics_json.end()) {
        physics_props.touching_wall_ = physics_json["touching_wall"].get<bool>();
    }
    if (physics_json.find("dashing") != physics_json.end()) {
        physics_props.dashing_ = physics_json["dashing"].get<bool>();
    }
    if (physics_json.find("can_jump") != physics_json.end()) {
        physics_props.can_jump_ = physics_json["can_jump"].get<bool>();
    }
    if (physics_json.find("can_dash") != physics_json.end()) {
        physics_props.can_dash_ = physics_json["can_dash"].get<bool>();
    }

    if (j.find("frame_timer") != j.end()) {
        state_props.frame_timer_ = j["frame_timer"].get<unsigned int>();
    }

    // Exceptions are not recoverable
    // Let propagate and crash
    nlohmann::json frame_names_array = j["frame_names"];

    for (auto it : frame_names_array) {
        state_props.frame_names_.push_back(it.get<std::string>());
    }

    state_utils::EntityContent entity_content = {physics_props, state_props, {}};

    if (j.find("spawn_entity") != j.end()) {
        entity_content.entity = j["spawn_entity"];
    }

    auto new_state = State(entity_content);

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

template class State<state_utils::EntityContent>;
template class State<std::vector<AI_CONDITION_TYPE>>;
