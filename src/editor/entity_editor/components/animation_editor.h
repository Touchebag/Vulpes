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

    void unpack(const nlohmann::json& animation_json, std::shared_ptr<sf::Texture> texture);
    nlohmann::json repack();

  private:
    void render();

    std::map<std::string, UnpackedAnimation> animations_;
    std::shared_ptr<sf::Texture> texture_;

    sf::Sprite render_sprite_;
    std::shared_ptr<sf::RenderTexture> render_texture_;
};

} // entity_editor
