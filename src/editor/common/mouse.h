#pragma once

#include <SFML/Graphics.hpp>

class Mouse {
  public:
    explicit Mouse(sf::RenderWindow& window);

    void saveMousePosition();

    std::pair<int, int> getMousePosition();
    std::pair<float, float> getMouseWorldPosition();

    std::pair<int, int> getMouseDistance();
    std::pair<float, float> getMouseWorldDistance();
  private:
    sf::RenderWindow& window_;

    std::pair<int, int> mouse_pos_;
    std::pair<float, float> mouse_world_pos_;
};
