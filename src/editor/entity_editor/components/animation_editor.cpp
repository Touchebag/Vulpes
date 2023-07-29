#include "animation_editor.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>

#include "components/animation.h"

#include "utils/log.h"

namespace entity_editor {

AnimationEditor::AnimationEditor() {
}

void AnimationEditor::unpack(const nlohmann::json& animation_json) {
    animations_.clear();

    for (auto [key, value] : animation_json.items()) {
        auto unpacked_anim = UnpackedAnimation(value);

        animations_.insert({key, unpacked_anim});
    }
}

nlohmann::json AnimationEditor::repack() {
    nlohmann::json packed_anims;

    for (auto it : animations_) {
        packed_anims[it.first] = it.second.repack();
    }
    return packed_anims;
}

void AnimationEditor::draw(sf::RenderWindow& /* window */) {
    if (active_) {
        ImGui::Begin("Animation", nullptr, 0
            | ImGuiWindowFlags_AlwaysAutoResize
            );

        ImGui::Image(*render_texture_);

        if (ImGui::Button("Close")) {
            active_ = false;
        }

        ImGui::End();
    }
}

} // entity_editor
