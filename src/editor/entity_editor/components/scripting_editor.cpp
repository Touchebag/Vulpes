#include "scripting_editor.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include "utils/log.h"

static const unsigned int IMGUI_BUFFER_SIZE = 500;

namespace entity_editor {

void ScriptingEditor::setActive() {
    active_ = true;
}

void ScriptingEditor::drawMenu(std::vector<std::string>& script) {
    if (active_) {
        ImGui::Begin("Scripting");

        for (long long unsigned int i = 0; i < script.size(); i++) {
            if (!edit_copy_string_.empty() && current_edit_ == i) {
                ImGui::InputText("", &(script[i]), IMGUI_BUFFER_SIZE);

                ImGui::SameLine();
                if (ImGui::Button("Done")) {
                    edit_copy_string_.clear();
                }

                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    // Restore old value on cancel
                    script[current_edit_] = edit_copy_string_;
                    edit_copy_string_.clear();
                }
            } else {
                if (ImGui::Button("Edit")) {
                    // Make a safety copy in case cancel
                    edit_copy_string_ = script[i];
                    current_edit_ = i;
                }
                ImGui::SameLine();
                ImGui::Text("%s", script[i].c_str());
            }
        }

        if (ImGui::Button("Close")) {
            active_ = false;
        }

        ImGui::End();
    }
}

} // entity_editor
