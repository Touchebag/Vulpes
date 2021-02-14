#pragma once

#include "rendering.h"

#include "SFML/Graphics.hpp"

class RenderableText : public RenderableEntity{
  public:
    RenderableText(std::weak_ptr<Transform> trans);

    void setText(const std::string& text);
    void setFont(sf::Font font);
    void setColor(sf::Color color) override;

    void render(sf::RenderTarget& target, float frame_fraction = 0.0f) override;

  private:
    sf::Font font_;
    sf::Text text_;
};
