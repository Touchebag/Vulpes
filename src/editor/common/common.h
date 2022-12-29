#pragma once

#include <memory>

#include "base_entity.h"
#include "system/system.h"

namespace editor_common {

std::shared_ptr<BaseEntity> makeHudText(std::pair<int, int> position);

enum class CurrentEditor {
    NONE,

    GAME,

    LEVEL,
    CUTSCENE,
};

void setCurrentEditor(CurrentEditor editor);
CurrentEditor getCurrentEditor();

void drawEditorMenu();

} // editor_common
