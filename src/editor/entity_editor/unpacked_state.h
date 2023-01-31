#pragma once

#include <nlohmann/json.hpp>

struct UnpackedState {
    UnpackedState(const std::string& name, nlohmann::json j);
    nlohmann::json repack();

    // Data

    std::string name;
    // Needed for renaming
    std::string old_name;

    // Event, state
    std::map<std::string, std::string> next_states;

    std::vector<std::string> templates;

    float x = 0.0;
    float y = 0.0;

  private:
    nlohmann::json original_json_;
};
