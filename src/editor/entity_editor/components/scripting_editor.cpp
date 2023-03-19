#include "scripting_editor.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include "utils/log.h"

namespace entity_editor {

void ScriptingEditor::setActive() {
    active_ = true;
}

void ScriptingEditor::drawMenu(std::vector<std::string>& script) {
    if (active_) {
        ImGui::Begin("Scripting", nullptr, 0
            | ImGuiWindowFlags_AlwaysAutoResize
            );

        for (int i = 0; i < static_cast<int>(script.size()); i++) {
            ImGui::PushID(std::to_string(i).c_str());
            if (current_edit_ == i) {
                if (ImGui::Button("Done")) {
                    script[i] = text_buf_;
                    current_edit_ = - 1;
                }

                ImGui::SameLine();
                ImGui::InputText("##script_text_input", text_buf_, IMGUI_BUFFER_SIZE);

                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    // Keep old value on cancel
                    current_edit_ = - 1;
                }
            } else {
                if (ImGui::Button("Edit")) {
                    strncpy(text_buf_, script[i].c_str(), IMGUI_BUFFER_SIZE - 1);
                    current_edit_ = i;
                }

                ImGui::SameLine();
                ImGui::Text("%s", script[i].c_str());
            }
            ImGui::PopID();
        }

        if (ImGui::Button("Close")) {
            active_ = false;
        }

        ImGui::End();
    }
}

} // entity_editor
