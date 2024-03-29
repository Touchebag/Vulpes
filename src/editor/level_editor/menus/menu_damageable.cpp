#include "menu_damageable.h"

#include "components/damageable/damageable.h"

#include "utils/log.h"

namespace damageable {

void toggleDamageable(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Damageable>()) {
        editor_env->current_entity->setComponent<Damageable>({});
    } else {
        editor_env->current_entity->setComponent<Damageable>(std::make_shared<Damageable>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // damageable

void MenuDamageable::drawMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Damageable");

    bool enabled = editor_env->current_entity->getComponent<Damageable>() ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        damageable::toggleDamageable(editor_env);
    }

    ImGui::End();
}
