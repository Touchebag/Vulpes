#include "state_view.h"

#include <set>

#include <imgui.h>
#include <imgui-SFML.h>
#include <misc/cpp/imgui_stdlib.h>

#include "common/mouse.h"

#include "utils/file.h"
#include "utils/log.h"

constexpr float WINDOW_WIDTH = 250.0f;
constexpr float WINDOW_HEIGHT = 30.0f;

namespace {

bool mouseCollides(const UnpackedState& state, const std::pair<float, float> mouse_pos) {
    if (mouse_pos.first  >= state.x - (WINDOW_WIDTH / 2.0f) &&
        mouse_pos.first  <= state.x + (WINDOW_WIDTH / 2.0f) &&
        mouse_pos.second >= state.y - (WINDOW_HEIGHT / 2.0f) &&
        mouse_pos.second <= state.y + (WINDOW_HEIGHT / 2.0f)) {

        return true;
    } else {
        return false;
    }
}

} // namespace

StateView::StateView() :
    font_(File::loadFont("arial.ttf").value()) {
}

void StateView::drawState(sf::RenderWindow& window, const std::string& state_name) {
    UnpackedState state = states_.at(state_name);

    // Background rectangle
    sf::RectangleShape rect({WINDOW_WIDTH, WINDOW_HEIGHT});

    auto size = rect.getLocalBounds();
    rect.setOrigin({size.width / 2.0f, size.height / 2.0f});
    rect.setPosition({state.x, state.y});

    rect.setFillColor(sf::Color(220, 220, 220));


    auto color = sf::Color(0, 0, 0);
    if (active_state_ == state_name) {
        color = sf::Color(255, 0, 0);
    } else if (hovered_state_ == state_name) {
        color = sf::Color(0, 0, 255);
    }

    rect.setOutlineThickness(10.0f);
    rect.setOutlineColor(color);

    window.draw(rect);

    // Text
    sf::Text text;

    text.setFont(font_);
    text.setString(state.name);

    text.setFillColor(sf::Color(0, 0, 0));

    text.setCharacterSize(24);
    text.setStyle(sf::Text::Bold);

    size = text.getLocalBounds();
    text.setOrigin({size.width / 2.0f, 16.0f});
    text.setPosition({state.x, state.y});

    window.draw(text);
}

void StateView::drawLines(sf::RenderWindow& window, const std::string& state_name) {
    UnpackedState state = states_.at(state_name);

    for (auto& it : state.next_states) {
        auto new_state = states_.at(it.second);

        auto color = sf::Color(0, 0, 0);

        if (active_state_ == state_name) {
            color = sf::Color(255, 0, 0);
        } else if (hovered_state_ == state_name) {
            color = sf::Color(0, 0, 255);
        }

        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(state.x, state.y), color),
            sf::Vertex(sf::Vector2f(new_state.x, new_state.y), color)
        };

        window.draw(line, 2, sf::Lines);
    }
}

void StateView::unpack(const nlohmann::json& state_file) {
    for (auto s : state_file["states"].items()) {
        states_.insert_or_assign(s.key(), UnpackedState(s.key(), s.value()));
    }

    positionStates();
}

void StateView::draw(sf::RenderWindow& window) {
    Mouse mouse{window};
    auto mouse_pos = mouse.getMouseWorldPosition();

    hovered_state_ = "";

    for (auto& it : states_) {
        if (mouseCollides(it.second, mouse_pos)) {
            hovered_state_ = it.first;

            break;
        }
    }

    // Draw lines first to appear behind boxes
    for (auto& it : states_) {
        drawLines(window, it.first);
    }

    for (auto& it : states_) {
        drawState(window, it.first);
    }
}

void StateView::positionStates() {
    constexpr int x_padding = 150.0;
    constexpr int y_padding = 100.0;

    std::vector<std::string> current_row;
    std::vector<std::string> next_row;
    std::set<std::string> checked_states;

    int current_row_index = 0;
    int current_col_index = 0;

    current_row.push_back("main");
    checked_states.insert("main");

    while (!current_row.empty()) {
        for (auto it : current_row) {

            auto& state = states_.at(it);

            for (auto ns : state.next_states) {
                if (checked_states.count(ns.second) == 0) {
                    next_row.push_back(ns.second);
                    checked_states.insert(ns.second);
                }
            }

            state.x = static_cast<float>(current_row_index) * (x_padding + WINDOW_WIDTH) + x_padding;
            state.y = static_cast<float>(current_col_index) * (y_padding + WINDOW_HEIGHT) + y_padding;

            current_col_index++;
        }

        current_row = next_row;
        next_row.clear();

        current_col_index = 0;
        current_row_index++;
    }
}

void StateView::handleMouseClick(std::pair<float, float> mouse_pos) {
    active_state_ = "";

    for (auto& it : states_) {
        if (mouseCollides(it.second, mouse_pos)) {
            active_state_ = it.first;

            break;
        }
    }
}
