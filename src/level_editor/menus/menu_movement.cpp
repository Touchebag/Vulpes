#include "menu_collision.h"

#include "components/collision/collision.h"

#include "utils/log.h"

namespace menu {

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

void componentMovementMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Component#Movement");

    auto move = editor_env->current_entity->getComponent<Movement>();
    bool enabled = move ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        movement::toggleMovement(editor_env);
    }

    ImGui::End();
}

} // menu
