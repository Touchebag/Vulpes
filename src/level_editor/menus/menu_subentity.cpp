#include "menu_subentity.h"

#include "components/subentity.h"

#include "utils/log.h"

namespace menu {

namespace subentity {

void toggleSubentity(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Subentity>()) {
        editor_env->current_entity->setComponent<Subentity>({});
    } else {
        editor_env->current_entity->setComponent<Subentity>(std::make_shared<Subentity>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // subentity

void componentSubentityMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Subentity");

    auto move = editor_env->current_entity->getComponent<Subentity>();
    bool enabled = move ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        subentity::toggleSubentity(editor_env);
    }

    ImGui::End();
}

} // menu
