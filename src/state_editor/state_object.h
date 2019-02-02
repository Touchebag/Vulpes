#pragma once

#include <SFML/Graphics.hpp>

constexpr float RECT_SIDE = 100.0;

class StateObject {
  public:
    void render(sf::RenderWindow& window);

    bool isMouseOver(sf::Vector2f pos);

    void move(sf::Vector2f pos);

  private:
    sf::Vector2f position_ = {0.0, 0.0};
    sf::RectangleShape rect_ = sf::RectangleShape(sf::Vector2f(RECT_SIDE, RECT_SIDE));
};
