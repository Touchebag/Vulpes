#pragma once

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "scripting_editor.h"
#include "unpacked_state.h"

namespace entity_editor {

class StateEditor {
  public:
    StateEditor();

    void unpack(const nlohmann::json& state_file);
    nlohmann::json repack();

    void draw(sf::RenderWindow& window);

    void handleKeyPress(sf::Event event);
    void handleMouseClick(sf::Event event, std::pair<float, float> mouse_pos);

  private:
    struct Line {
        sf::Vector2f from;
        sf::Vector2f to;

        float thickness = 1.0;
        sf::Color color = {0, 0, 0};
    };

    std::vector<Line> normal_lines_;
    std::vector<Line> selected_lines_;
    std::vector<Line> hovered_lines_;

    void drawState(sf::RenderWindow& window, const std::string& state_name);
    void drawEditWindow();

    void drawLineRect(sf::RenderWindow& window, Line line);
    void createLines(const std::string& state_name);

    void positionStates();

    sf::Font font_;

    std::string hovered_state_ = "";
    std::string active_state_ = "";

    struct ActiveState {
        static const int IMGUI_INPUT_BUFFER_SIZE = 500;

        char name[IMGUI_INPUT_BUFFER_SIZE];
    };

    entity_editor::ScriptingEditor scripting_editor_{};

    ActiveState active_state_data_ = ActiveState{};

    std::unordered_map<std::string, UnpackedState> states_;
    std::unordered_map<std::string, UnpackedState> templates_;
};

} // entity_editor
