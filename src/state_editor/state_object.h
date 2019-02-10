#pragma once

#include <SFML/Graphics.hpp>

#include "json.hpp"

constexpr float RECT_SIDE = 100.0;

class StateObject {
  public:
    StateObject(nlohmann::json state, sf::Font& font);

    void render(sf::RenderWindow& window);

    bool isMouseOver(sf::Vector2f pos);

    void move(sf::Vector2f pos);

    void renderStateText(sf::RenderWindow& window);

    sf::Vector2f position_ = {0.0, 0.0};

  private:
    void renderStateTransitions(sf::RenderWindow& window);

    sf::RectangleShape rect_ = sf::RectangleShape(sf::Vector2f(RECT_SIDE, RECT_SIDE));

    nlohmann::json state_;
    sf::Font& font_;
};
