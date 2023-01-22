#include "state_view.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <misc/cpp/imgui_stdlib.h>

#include "utils/log.h"

void StateView::drawState(const std::string& state_name) {
    UnpackedState state = states_.at(state_name);

    ImGui::Begin(state.name.c_str(), nullptr, 0
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_AlwaysAutoResize
            );

    ImGui::Text("Name");
    ImGui::SameLine();
    if (ImGui::InputText("##Name", &state.name, 0
                | ImGuiInputTextFlags_EnterReturnsTrue)) {
        states_.erase(state.old_name);
        states_.insert_or_assign(state.name, state);
    }

    ImGui::End();

    (void)state;
}

void StateView::unpack(const nlohmann::json& state_file) {
    for (auto s : state_file["states"].items()) {
        states_.insert_or_assign(s.key(), UnpackedState(s.key(), s.value()));
    }
}

void StateView::update() {
    for (auto& it : states_) {
        drawState(it.first);
    }

    (void)window;
}
