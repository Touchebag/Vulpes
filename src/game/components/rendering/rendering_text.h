#pragma once

#include "rendering.h"

#include "SFML/Graphics.hpp"

class RenderableText : public RenderableEntity{
  public:
    RenderableText(std::weak_ptr<Transform> trans);

    void setText(const std::string& text);
    void setFont(sf::Font font);
    void setColor(sf::Color color);

    void render(sf::RenderWindow& window) override;

  private:
    sf::Font font_;
    sf::Text text_;
};
