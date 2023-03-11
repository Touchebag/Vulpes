#include "menu_scripting.h"

#include "components/scripting/script.h"

#include "utils/log.h"

namespace {

void toggleScripting(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Scripting>()) {
        editor_env->current_entity->setComponent<Scripting>({});
    } else {
        editor_env->current_entity->setComponent<Scripting>(std::make_shared<Scripting>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // namespace

void MenuScripting::drawMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Scripting");

    bool enabled = editor_env->current_entity->getComponent<Scripting>() ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        toggleScripting(editor_env);
    }

    ImGui::End();
}
