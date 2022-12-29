#include "top_menu.h"

#include "common/editor_render.h"
#include "cutscene_editor/cutscene_editor_main.h"
#include "common/common.h"

using editor_common::CurrentEditor;

void TopMenu::draw(sf::RenderWindow& window) {
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
        for (auto entry : menus_) {
            if (ImGui::BeginMenu(entry.first.c_str())) {
                entry.second();
                ImGui::EndMenu();
            }
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void TopMenu::addMenu(const std::string& name, std::function<void(void)> entry) {
    menus_.push_back({name, entry});
}

void TopMenu::clearMenus() {
    menus_.clear();
}

