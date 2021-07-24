#include "menu_transform.h"

#include "components/transform.h"

#include "utils/log.h"

namespace transform {

void toggleTransform(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Transform>()) {
        editor_env->current_entity->setComponent<Transform>({});
    } else {
        editor_env->current_entity->setComponent<Transform>(std::make_shared<Transform>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // transform

void MenuTransform::drawMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Transform");

    bool enabled = editor_env->current_entity->getComponent<Transform>() ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        transform::toggleTransform(editor_env);
    }

    ImGui::End();
}
