#pragma once

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "unpacked_state.h"

class StateView {
  public:
    void unpack(const nlohmann::json& state_file);

    void draw(sf::RenderWindow& window);

  private:
    void drawState(sf::RenderWindow& window, const std::string& state_name);
    void positionStates();

    std::unordered_map<std::string, UnpackedState> states_;
};
