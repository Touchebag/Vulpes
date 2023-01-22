#pragma once

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "unpacked_state.h"

class StateView {
  public:
    void unpack(const nlohmann::json& state_file);

    void update();

  private:
    void drawState(const std::string& state_name);

    std::unordered_map<std::string, UnpackedState> states_;
};
