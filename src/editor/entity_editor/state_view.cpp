#include "state_view.h"

#include <set>

#include <imgui.h>
#include <imgui-SFML.h>
#include <misc/cpp/imgui_stdlib.h>

#include "utils/log.h"

constexpr int WINDOW_WIDTH = 150;
constexpr int WINDOW_HEIGHT = 100;

void StateView::drawState(sf::RenderWindow& window, const std::string& state_name) {
    UnpackedState state = states_.at(state_name);

    sf::RectangleShape rect({static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)});

    rect.setPosition({static_cast<float>(state.x), static_cast<float>(state.y)});

    rect.setFillColor(sf::Color(220, 220, 220));

    rect.setOutlineThickness(10.0f);
    rect.setOutlineColor(sf::Color(0, 0, 0));

    window.draw(rect);
}

void StateView::unpack(const nlohmann::json& state_file) {
    for (auto s : state_file["states"].items()) {
        states_.insert_or_assign(s.key(), UnpackedState(s.key(), s.value()));
    }

    positionStates();
}

void StateView::draw(sf::RenderWindow& window) {
    for (auto& it : states_) {
        drawState(window, it.first);
    }
}

void StateView::positionStates() {
    constexpr int x_padding = 50;
    constexpr int y_padding = 50;

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

            state.x = current_row_index * (x_padding + WINDOW_WIDTH) + x_padding;
            state.y = current_col_index * (y_padding + WINDOW_HEIGHT) + x_padding;

            current_col_index++;
        }

        current_row = next_row;
        next_row.clear();

        current_col_index = 0;
        current_row_index++;
    }
}
