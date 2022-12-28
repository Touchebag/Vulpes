#include "top_menu.h"

#include "common/editor_render.h"
#include "cutscene_editor/cutscene_editor_main.h"
#include "common/common.h"

namespace menu {

using editor_common::CurrentEditor;

void topMenu(sf::RenderWindow& window, std::shared_ptr<EditorEnvironment> /* editor_env */) {
    ImGui::SetNextWindowPos({-1, 0});
    ImGui::SetNextWindowSize({static_cast<float>(window.getSize().x + 2), 0});

    ImGui::Begin("Top Menu", nullptr, 0
            | ImGuiWindowFlags_MenuBar
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoBackground
            );

    if (ImGui::BeginMenuBar()) {

        if (ImGui::BeginMenu("File")) {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Editor")) {
            bool l_ed = false;
            ImGui::Selectable("Level", &l_ed);
            if (l_ed) {
                editor_common::setCurrentEditor(CurrentEditor::LEVEL);
            }

            bool c_ed = false;
            ImGui::Selectable("Cutscene", &c_ed);
            if (c_ed) {
                editor_common::setCurrentEditor(CurrentEditor::CUTSCENE);
            }

            ImGui::EndMenu();
        }

        if (editor_common::getCurrentEditor() == CurrentEditor::LEVEL) {
            if (ImGui::BeginMenu("Rendering")) {
                if (auto render = std::dynamic_pointer_cast<EditorRender>(System::getRender())) {
                    bool parallax_enabled = render->getParallaxEnabled();

                    ImGui::Checkbox("Render Hitboxes", &(render->render_hitboxes_));
                    ImGui::Checkbox("Render Entrances", &(render->render_entrances_));
                    ImGui::Checkbox("Parallax enabled", &(parallax_enabled));

                    render->setParallaxEnabled(parallax_enabled);
                } else {
                    ImGui::Text("ERROR: Failed to cast render instance");
                }
                ImGui::EndMenu();
            }
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

} // menu
