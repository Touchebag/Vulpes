#include "menu_movement.h"

#include "components/movement.h"

#include "utils/log.h"

namespace movement {

void toggleMovement(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Movement>()) {
        editor_env->current_entity->setComponent<Movement>({});
    } else {
        editor_env->current_entity->setComponent<Movement>(std::make_shared<Movement>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // movement

void MenuMovement::drawMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Movement");

    bool enabled = editor_env->current_entity->getComponent<Movement>() ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        movement::toggleMovement(editor_env);
    }

    ImGui::End();
}
