#include "cutscene_top_menu.h"

#include "common/common.h"

namespace {

void fileMenu() {
}

} // namespace

namespace cutscene_top_menu {

TopMenu createMenus() {
    TopMenu top_menu;

    top_menu.addMenu("File", &fileMenu);
    top_menu.addMenu("Editor", &editor_common::drawEditorMenu);

    return top_menu;
}

} // cutscene_top_menu
