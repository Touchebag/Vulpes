#pragma once

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "unpacked_state.h"

class StateView {
  public:
    StateView();

    void unpack(const nlohmann::json& state_file);
    nlohmann::json repack();

    void draw(sf::RenderWindow& window);

    void handleMouseClick(std::pair<float, float> mouse_pos);

  private:
    void drawState(sf::RenderWindow& window, const std::string& state_name);
    void drawLines(sf::RenderWindow& window, const std::string& state_name);

    void positionStates();

    sf::Font font_;

    std::string hovered_state_ = "";
    std::string active_state_ = "";

    std::unordered_map<std::string, UnpackedState> states_;
    std::unordered_map<std::string, UnpackedState> templates_;
};
