#include "unpacked_state.h"

#include "utils/log.h"

UnpackedState::UnpackedState(const std::string& state_name, nlohmann::json j) {
    this->name = state_name;
    this->old_name = state_name;

    if (j.contains("next_states")) {
        for (auto it : j["next_states"]) {
            std::string event = it["event"];
            std::string state = it["state"];
            next_states.insert_or_assign(event, state);
        }
    }

    if (j.contains("templates")) {
        for (auto it : j["templates"]) {
            templates.push_back(it);
        }
    }
}
