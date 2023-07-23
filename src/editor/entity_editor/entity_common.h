#pragma once

namespace entity_editor {

enum class EntityEditorView {
    ANIMATION,
    STATE,
};

class EntityEditorCommon {
  public:
    static void setCurrentEditorView(EntityEditorView view) {
        getInstance().current_editor_viev = view;
    }

    static EntityEditorView getCurrentEditorView() {
        return getInstance().current_editor_viev;
    }

  private:
    EntityEditorCommon() = default;

    static EntityEditorCommon& getInstance() {
        static EntityEditorCommon instance;
        return instance;
    }

    EntityEditorView current_editor_viev = EntityEditorView::STATE;
};

} // entity_editor
