#pragma once

#include <memory>

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "unpacked_animation.h"
#include "components/scripting/instructions.h"

namespace entity_editor {

class AnimationEditor {
  public:
    AnimationEditor();

    void draw(sf::RenderWindow& window);

    void unpack(const nlohmann::json& animation_json);
    nlohmann::json repack();

  private:
    bool active_ = false;

    std::map<std::string, UnpackedAnimation> animations_;
    std::shared_ptr<sf::RenderTexture> render_texture_;
};

} // entity_editor
