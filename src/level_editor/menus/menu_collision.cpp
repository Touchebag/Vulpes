#include "menu_collision.h"

#include "components/collision/collision.h"

#include "utils/log.h"

namespace collision {

void toggleCollsion(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Collision>()) {
        editor_env->current_entity->setComponent<Collision>({});
    } else {
        editor_env->current_entity->setComponent<Collision>(std::make_shared<Collision>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // collision

void MenuCollision::drawMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Collision");

    bool enabled = editor_env->current_entity->getComponent<Collision>() ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        collision::toggleCollsion(editor_env);
    }

    ImGui::End();
}
