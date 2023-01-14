#pragma once

#include <nlohmann/json.hpp>

class UnpackedState {
  public:
    UnpackedState(std::string name, nlohmann::json j);

    // Data
    std::string name;
    // Needed for renaming
    std::string old_name;

    int x;
    int y;
};
