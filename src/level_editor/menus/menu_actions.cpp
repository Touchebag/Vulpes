#include "menu_actions.h"

#include "components/actions/actions.h"

#include "utils/log.h"

namespace menu {

namespace actions {

void toggleActions(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Actions>()) {
        editor_env->current_entity->setComponent<Actions>({});
    } else {
        editor_env->current_entity->setComponent<Actions>(std::make_shared<Actions>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // actions

void componentActionsMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Actions");

    auto move = editor_env->current_entity->getComponent<Actions>();
    bool enabled = move ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        actions::toggleActions(editor_env);
    }

    ImGui::End();
}

} // menu
