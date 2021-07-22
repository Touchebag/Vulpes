#include "menu_death.h"

#include "components/death.h"

#include "utils/log.h"

namespace menu {

namespace death {

void toggleDeath(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Death>()) {
        editor_env->current_entity->setComponent<Death>({});
    } else {
        editor_env->current_entity->setComponent<Death>(std::make_shared<Death>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // death

void componentDeathMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Death");

    auto move = editor_env->current_entity->getComponent<Death>();
    bool enabled = move ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        death::toggleDeath(editor_env);
    }

    ImGui::End();
}

} // menu
