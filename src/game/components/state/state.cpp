#include "state.h"

#include "utils/log.h"

#include "components/actions/actions.h"
#include "components/scripting/program.h"
#include "components/physics/physics.h"
#include "components/component_store.h"


template <class T>
State<T>::State(const T& data) :
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
State<state_utils::EntityContent> State<state_utils::EntityContent>::loadStateFromJson(nlohmann::json j, std::shared_ptr<ComponentStore> components) {
    state_utils::PhysicsProperties physics_props;
    state_utils::StateProperties state_props;
    nlohmann::json entity;
    std::vector<Program> script;
    std::vector<Program> script_on_enter;
    std::vector<Program> script_on_exit;
    std::optional<PhysicsConstants> physics_consts = std::nullopt;

    nlohmann::json props_json = j["properties"];

    // If an option is not found use default
    if (props_json.contains("movement_locked_x")) {
        physics_props.movement_locked_x_ = props_json["movement_locked_x"].get<bool>();
    }
    if (props_json.contains("movement_locked_y")) {
        physics_props.movement_locked_y_ = props_json["movement_locked_y"].get<bool>();
    }
    if (props_json.contains("direction_locked")) {
        physics_props.direction_locked_ = props_json["direction_locked"].get<bool>();
    }
    if (props_json.contains("touching_ground")) {
        physics_props.touching_ground_ = props_json["touching_ground"].get<bool>();
    }
    if (props_json.contains("touching_wall")) {
        physics_props.touching_wall_ = props_json["touching_wall"].get<bool>();
    }

    if (j.contains("frame_timer")) {
        state_props.frame_timer_ = j["frame_timer"].get<unsigned int>();
    }

    // Exceptions are not recoverable
    // Let propagate and crash
    if (j.contains("animation")) {
        state_props.animation_name = j["animation"];
    }

    if (j.contains("collideables")) {
        state_props.collideables = j["collideables"];
    }

    if (j.contains("spawn_entity")) {
        entity = j["spawn_entity"];
    }

    if (j.contains("script")) {
        for (auto it : j["script"]) {
            auto program = Program::loadProgram(it.get<std::string>());
            // TODO Check correct type

            auto meta = program.getMetaData();
            if (meta == Program::MetaData::ON_ENTER) {
                script_on_enter.push_back(program);
            } else if (meta == Program::MetaData::ON_EXIT) {
                script_on_exit.push_back(program);
            } else {
                script.push_back(program);
            }
        }
    }

    if (j.contains("physics_constants")) {
        PhysicsConstants phys_const;
        if (auto physics_component = components->getComponent<Physics>()) {
            phys_const = physics_component->getPhysicsConstants();
        }

        physics_consts = {Physics::loadConstantsFromJson(j["physics_constants"], phys_const)};
    }

    state_utils::EntityContent entity_content;
    entity_content.physics_props     = physics_props;
    entity_content.state_props       = state_props;
    entity_content.entity            = entity;
    entity_content.script            = script;
    entity_content.script_on_enter   = script_on_enter;
    entity_content.script_on_exit    = script_on_exit;
    entity_content.physics_constants = physics_consts;

    auto new_state = State(entity_content);

    new_state.loadNextStateListFromJson(j["next_states"]);

    return new_state;
}

template class State<state_utils::EntityContent>;
