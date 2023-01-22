#pragma once

#include <SFML/Graphics.hpp>

#include "common/mouse.h"

namespace entity_editor {

class EditorView {
  public:
    explicit EditorView(sf::RenderWindow& window);

    void update();

    void handleMouseEvent(sf::Event event);
    void handleKeyEvent(sf::Event event);

    void setView(std::pair<float, float> pos, std::pair<float, float> size);

  private:
    sf::RenderWindow& window_;

    Mouse mouse_;

    bool dragging_view_ = false;

    std::pair<float, float> initial_mouse_pos_;

    std::pair<float, float> center_position_ = {0.0, 0.0};
    std::pair<float, float> size_ = {100.0, 100.0};
};

} // entity_editor
