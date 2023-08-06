#include "animation_editor.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>

#include "components/animation.h"

#include "utils/log.h"

namespace entity_editor {

AnimationEditor::AnimationEditor() :
    render_texture_(std::make_shared<sf::RenderTexture>()) {
    render_texture_->create(500, 500);

    auto view = render_texture_->getView().getSize();

    render_sprite_.setPosition(view.x / 2, view.y / 2);
}

void AnimationEditor::unpack(const nlohmann::json& animation_json, std::shared_ptr<sf::Texture> texture) {
    animations_.clear();

    for (auto [key, value] : animation_json.items()) {
        auto unpacked_anim = UnpackedAnimation(value);

        animations_.insert({key, unpacked_anim});
    }

    texture_ = texture;
}

nlohmann::json AnimationEditor::repack() {
    nlohmann::json packed_anims;

    for (auto it : animations_) {
        packed_anims[it.first] = it.second.repack();
    }
    return packed_anims;
}

void AnimationEditor::updateTextureCoords() {
    render_sprite_.setTexture(*texture_);

    auto animation = animations_.at("idle");
    auto current_frame = animation.frame_list.at(0);
    auto rect = animation.sprite_rect_map.at(current_frame.name);

    render_sprite_.setTextureRect(sf::IntRect(rect.second.x, rect.second.y, rect.second.width, rect.second.height));

    auto view = render_texture_->getView().getSize();
    auto sprite_size = render_sprite_.getLocalBounds();
    render_sprite_.setOrigin(static_cast<float>(sprite_size.width / 2.0), static_cast<float>(sprite_size.height / 2.0));
    render_sprite_.setScale(view.x / sprite_size.width, view.y / sprite_size.height);
}

void AnimationEditor::render() {
    render_texture_->clear(sf::Color(0, 0, 0, 255));
    render_texture_->draw(render_sprite_);
    render_texture_->display();
}

void AnimationEditor::draw(sf::RenderWindow& /* window */) {
    updateTextureCoords();
    render();

    ImGui::Begin("Animation", nullptr, 0
        | ImGuiWindowFlags_AlwaysAutoResize
        );

    ImGui::Image(*render_texture_);

    ImGui::End();
}

} // entity_editor
