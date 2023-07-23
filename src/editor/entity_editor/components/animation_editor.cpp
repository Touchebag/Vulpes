#include "animation_editor.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>

#include "components/animation.h"

#include "utils/log.h"

namespace entity_editor {

AnimationEditor::AnimationEditor() {
    // render_texture_ = std::make_shared<sf::RenderTexture>();
    //
    // render_texture_->create(200, 200);
    // render_texture_->clear();
    // render_texture_->display();
    //
    // auto a = Animation::createFromJson({}, {}, {});
    // a->getFrameData();
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
