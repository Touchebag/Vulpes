#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "components/scripting/instructions.h"

namespace entity_editor {

class AnimationEditor {
  public:
    AnimationEditor();

    void draw(sf::RenderWindow& window);

  private:
    bool active_ = false;

    std::shared_ptr<sf::RenderTexture> render_texture_;
};

} // entity_editor
