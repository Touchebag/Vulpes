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
    void updateTextureCoords();
    void setAnimation(const std::string& name);

    std::map<std::string, std::shared_ptr<UnpackedAnimation>> animations_;
    std::shared_ptr<sf::Texture> texture_;

    sf::Sprite render_sprite_;
    std::shared_ptr<sf::RenderTexture> render_texture_;

    std::shared_ptr<UnpackedAnimation> current_animation_;
    int current_frame_ = 0;
    float current_scaling_ = 1.0;

    bool autoplay_ = false;

    sf::Time time_rendered_;
    sf::Clock render_clock_;
};

} // entity_editor
