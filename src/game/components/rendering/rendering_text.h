#pragma once

#include "rendering.h"

#include "SFML/Graphics.hpp"

class RenderingText : public Rendering {
  public:
    RenderingText(std::weak_ptr<ComponentStore> components);

    void setText(const std::string& text);
    void setFont(sf::Font font);
    void setColor(sf::Color color) override;

    void render(sf::RenderTarget& target, double frame_fraction = 0.0) override;

  private:
    sf::Font font_;
    sf::Text text_;
};
