#include "level_top_menu.h"

#include "system/system.h"
#include "common/editor_render.h"
#include "common/common.h"

namespace {

void fileMenu() {
}

void renderingMenu() {
    if (auto render = std::dynamic_pointer_cast<EditorRender>(System::getRender())) {
        bool parallax_enabled = render->getParallaxEnabled();

        ImGui::Checkbox("Render Hitboxes", &(render->render_hitboxes_));
        ImGui::Checkbox("Render Entrances", &(render->render_entrances_));
        ImGui::Checkbox("Parallax enabled", &(parallax_enabled));

        render->setParallaxEnabled(parallax_enabled);
    } else {
        ImGui::Text("ERROR: Failed to cast render instance");
    }
}

} // namespace

namespace level_top_menu {

TopMenu createMenus() {
    TopMenu top_menu;

    top_menu.addMenu("File", &fileMenu);
    top_menu.addMenu("Editor", &editor_common::drawEditorMenu);
    top_menu.addMenu("Rendering", &renderingMenu);

    return top_menu;
}

} // level_top_menu
