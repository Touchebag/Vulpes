#include "state_view.h"

#include <set>

#include <imgui.h>
#include <imgui-SFML.h>

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

void StateView::drawLineRect(sf::RenderWindow& window, StateView::Line line) {
    sf::Vertex vertices[4];

    sf::Vector2f dist = line.to - line.from;
    sf::Vector2f unit_vector = dist / std::sqrt(dist.x * dist.x + dist.y * dist.y);
    sf::Vector2f unit_perpendicular(-unit_vector.y, unit_vector.x);

    sf::Vector2f thickness_offset = (line.thickness / 2.f) * unit_perpendicular;

    // Used to prevent bi-directional lines from overlapping
    sf::Vector2f position_offset = (10.0f) * unit_perpendicular;

    vertices[0].position = line.from + thickness_offset + position_offset;
    vertices[1].position = line.to + thickness_offset + position_offset;
    vertices[2].position = line.to - thickness_offset + position_offset;
    vertices[3].position = line.from - thickness_offset + position_offset;

    for (int i=0; i<4; ++i)
        vertices[i].color = line.color;

    window.draw(vertices, 4, sf::Quads);
}

void StateView::createLines(const std::string& state_name) {
    UnpackedState state = states_.at(state_name);

    auto next_states = state.next_states;
    for (auto template_name : state.templates) {
        auto templ = templates_.at(template_name);
        for (auto ns : templ.next_states) {
            next_states.insert({ns.first, ns.second});
        }
    }

    for (auto& it : next_states) {
        auto new_state = states_.at(it.second);

        Line line;
        if (active_state_ == state_name) {
            line.from = {state.x, state.y};
            line.to = {new_state.x, new_state.y};
            line.color = sf::Color(255, 0, 0);
            line.thickness = 5.0f;
            selected_lines_.push_back(line);
        } else if (hovered_state_ == state_name) {
            line.from = {state.x, state.y};
            line.to = {new_state.x, new_state.y};
            line.color = sf::Color(0, 0, 255);
            line.thickness = 5.0f;
            hovered_lines_.push_back(line);
        } else {
            line.from = {state.x, state.y};
            line.to = {new_state.x, new_state.y};
            line.color = sf::Color(0, 0, 0);
            line.thickness = 2.0f;
            normal_lines_.push_back(line);
        }
    }
}

void StateView::unpack(const nlohmann::json& state_file) {
    for (auto s : state_file["templates"].items()) {
        templates_.insert_or_assign(s.key(), UnpackedState(s.key(), s.value()));
    }

    for (auto s : state_file["states"].items()) {
        states_.insert_or_assign(s.key(), UnpackedState(s.key(), s.value()));
    }

    positionStates();
}

nlohmann::json StateView::repack() {
    nlohmann::json j;

    nlohmann::json templates;
    for (auto it : templates_) {
        templates[it.second.name] = it.second.repack();
    }
    j["templates"] = templates;

    nlohmann::json states;
    for (auto it : states_) {
        states[it.second.name] = it.second.repack();
    }
    j["states"] = states;

    return j;
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
    normal_lines_.clear();
    selected_lines_.clear();
    hovered_lines_.clear();

    for (auto& it : states_) {
        createLines(it.first);
    }

    for (auto it : normal_lines_) {
        drawLineRect(window, it);
    }

    for (auto it : selected_lines_) {
        drawLineRect(window, it);
    }

    for (auto it : hovered_lines_) {
        drawLineRect(window, it);
    }

    // Draw states on top
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

            auto next_states = state.next_states;
            for (auto template_name : state.templates) {
                auto templ = templates_.at(template_name);
                for (auto ns : templ.next_states) {
                    next_states.insert({ns.first, ns.second});
                }
            }

            for (auto ns : next_states) {
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

void StateView::handleKeyPress(sf::Event event) {
    switch (event.key.code) {
        case sf::Keyboard::Key::Escape:
            active_state_ = "";
            break;
        default:
            break;
    }
}

void StateView::handleMouseClick(sf::Event event, std::pair<float, float> mouse_pos) {
    if (event.mouseButton.button == sf::Mouse::Button::Left) {
        for (auto& it : states_) {
            if (mouseCollides(it.second, mouse_pos)) {
                active_state_ = it.first;

                break;
            }
        }
    }
}
