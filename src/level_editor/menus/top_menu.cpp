#include "top_menu.h"

#include "editor_render.h"
#include "cutscene_editor.h"

namespace menu {

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
            bool open_cutscene_editor = false;
            ImGui::Selectable("Cutscene", &open_cutscene_editor);

            if (open_cutscene_editor) {
                cutsceneEditorMain(window);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Rendering")) {
            if (auto render = std::dynamic_pointer_cast<EditorRender>(System::getRender())) {
                ImGui::Checkbox("Render Hitboxes", &(render->render_hitboxes_));
                ImGui::Checkbox("Render Entrances", &(render->render_entrances_));
            } else {
                ImGui::Text("ERROR: Failed to cast render instance");
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

} // menu
