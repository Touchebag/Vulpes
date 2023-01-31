#include "unpacked_state.h"

#include "utils/log.h"

UnpackedState::UnpackedState(const std::string& state_name, nlohmann::json j) :
    name(state_name),
    old_name(state_name),
    original_json_(j)
{

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

nlohmann::json UnpackedState::repack() {
    nlohmann::json j;

    if (original_json_.contains("physics_constants")) {
        j["physics_constants"] = original_json_["physics_constants"];
    }

    if (original_json_.contains("properties")) {
        j["properties"] = original_json_["properties"];
    }

    if (original_json_.contains("frame_timer")) {
        j["frame_timer"] = original_json_["frame_timer"];
    }

    if (original_json_.contains("spawn_entity")) {
        j["spawn_entity"] = original_json_["spawn_entity"];
    }

    if (original_json_.contains("collideables")) {
        j["collideables"] = original_json_["collideables"];
    }

    if (original_json_.contains("ai")) {
        j["ai"] = original_json_["ai"];
    }

    if (original_json_.contains("animation")) {
        j["animation"] = original_json_["animation"];
    }

    if (!templates.empty()) {
        std::vector<std::string> j_templates;
        for (auto it : templates) {
            j_templates.push_back(it);
        }
        j["templates"] = j_templates;
    }

    if (!next_states.empty()) {
        std::vector<nlohmann::json> j_next_states;
        for (auto it : next_states) {
            nlohmann::json tmp;
            tmp["event"] = it.first;
            tmp["state"] = it.second;

            j_next_states.push_back(tmp);
        }
        j["next_states"] = j_next_states;
    }

    return j;
}
