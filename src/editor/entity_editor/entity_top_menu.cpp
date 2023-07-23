#include "entity_top_menu.h"

#include "common/common.h"
#include "entity_editor/entity_common.h"
#include "entity_editor/unpacked_entity.h"

namespace {

void fileMenu() {
}

void componentMenu() {
    bool c_anim = false;
    ImGui::Selectable("Animation", &c_anim);
    if (c_anim) {
        entity_editor::EntityEditorCommon::setCurrentEditorView(entity_editor::EntityEditorView::ANIMATION);
    }

    bool c_state = false;
    ImGui::Selectable("State", &c_state);
    if (c_state) {
        entity_editor::EntityEditorCommon::setCurrentEditorView(entity_editor::EntityEditorView::STATE);
    }
}

} // namespace

namespace entity_top_menu {

TopMenu createMenus() {
    TopMenu top_menu;

    top_menu.addMenu("File", &fileMenu);
    top_menu.addMenu("Editor", &editor_common::drawEditorMenu);
    top_menu.addMenu("Component", &componentMenu);

    return top_menu;
}

} // cutscene_top_menu
