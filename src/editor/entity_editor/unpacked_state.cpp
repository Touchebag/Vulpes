#include "unpacked_state.h"

#include "utils/log.h"

UnpackedState::UnpackedState(const std::string& name, nlohmann::json j) {
    this->name = name;
    this->old_name = name;

    // TODO Templates

    if (j.contains("next_states")) {
        for (auto it : j["next_states"]) {
            std::string event = it["event"];
            std::string state = it["state"];
            next_states.insert_or_assign(event, state);
        }
    }
}
