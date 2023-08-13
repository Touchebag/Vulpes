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

    render_texture_->setView(sf::View(sf::FloatRect(-250, -250, 500, 500)));

    render_sprite_.setPosition(0.0f, 0.0f);
}

void AnimationEditor::unpack(const nlohmann::json& animation_json, std::shared_ptr<sf::Texture> texture) {
    animations_.clear();

    for (auto [key, value] : animation_json.items()) {
        auto unpacked_anim = std::make_shared<UnpackedAnimation>(key, value);

        animations_.insert({key, unpacked_anim});
    }

    setAnimation("idle");
    current_frame_ = 0;

    texture_ = texture;
}

void AnimationEditor::setAnimation(const std::string& name) {
    current_animation_ = animations_.at(name);

    int max_width = 0;
    int max_height = 0;
    for (auto& it : current_animation_->frame_list) {
        auto rect = current_animation_->sprite_rect_map.at(it.name);
        max_width = std::max(max_width, rect.second.width + 2 * (it.x_offset ? abs(it.x_offset.value()) : 0));
        max_height = std::max(max_height, rect.second.height + 2 * (it.y_offset ? abs(it.y_offset.value()) : 0));
    }

    auto render_size = render_texture_->getView().getSize();
    float x_scale = static_cast<float>(render_size.x) / static_cast<float>(max_width);
    float y_scale = static_cast<float>(render_size.y) / static_cast<float>(max_height);
    current_scaling_ = std::min(x_scale, y_scale);
}

nlohmann::json AnimationEditor::repack() {
    nlohmann::json packed_anims;

    for (auto it : animations_) {
        packed_anims[it.first] = it.second->repack();
    }
    return packed_anims;
}

void AnimationEditor::updateTextureCoords() {
    if (autoplay_) {
        time_rendered_ += render_clock_.getElapsedTime();
        render_clock_.restart();

        if (time_rendered_.asMilliseconds() >= 16) {
            current_frame_++;
            current_frame_ %= static_cast<int>(current_animation_->frame_list.size());
            time_rendered_ -= sf::milliseconds(16);
        }
    }

    render_sprite_.setTexture(*texture_);

    auto& frame = current_animation_->frame_list.at(current_frame_);
    auto rect = current_animation_->sprite_rect_map.at(frame.name);

    render_sprite_.setTextureRect(sf::IntRect(rect.second.x, rect.second.y, rect.second.width, rect.second.height));

    auto sprite_size = render_sprite_.getLocalBounds();
    render_sprite_.setOrigin(static_cast<float>(sprite_size.width / 2.0), static_cast<float>(sprite_size.height / 2.0));

    // Offset
    render_sprite_.setPosition(frame.x_offset ? static_cast<float>(frame.x_offset.value()) : 0.0f,
                               frame.y_offset ? static_cast<float>(frame.y_offset.value()) : 0.0f);

    // Scale
    render_sprite_.setScale(current_scaling_ * static_cast<float>(frame.x_scale ? frame.x_scale.value() : 1.0),
                            current_scaling_ * static_cast<float>(frame.y_scale ? frame.y_scale.value() : 1.0));
}

void AnimationEditor::render() {
    render_texture_->clear(sf::Color(0, 0, 0, 255));
    render_texture_->draw(render_sprite_);
    render_texture_->display();
}

void AnimationEditor::draw(sf::RenderWindow& /* window */) {
    if (current_animation_) {
        updateTextureCoords();
        render();

        ImGui::Begin("Animation", nullptr, 0
            | ImGuiWindowFlags_AlwaysAutoResize
            );

        ImGui::Text("Animation");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##AnimationList", current_animation_->name.c_str())) {
            for (auto& it : animations_) {
                bool is_selected = (current_animation_->name == it.first);
                if (ImGui::Selectable(it.first.c_str(), is_selected)) {
                    setAnimation(it.first);
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Image(*render_texture_);

        ImGui::SameLine();
        if (ImGui::BeginListBox("##FrameList", ImVec2(200.0f, static_cast<float>(render_texture_->getSize().y)))) {
            for (int i = 0; i < static_cast<int>(current_animation_->frame_list.size()); i++) {
                auto& frame = current_animation_->frame_list.at(i);
                auto name = frame.name + "##" + std::to_string(i);

                bool is_selected = (current_frame_ == i);
                if (ImGui::Selectable(name.c_str(), is_selected)) {
                    current_frame_ = i;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }

        if(ImGui::Button("Play")) {
            time_rendered_ = sf::milliseconds(0);
            autoplay_ = !autoplay_;
        }

        ImGui::End();
    }
}

} // entity_editor
