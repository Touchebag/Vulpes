#include "menu_animation.h"

#include "components/animation.h"

#include "utils/log.h"

namespace menu {

namespace animation {

void toggleAnimation(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Animation>()) {
        editor_env->current_entity->setComponent<Animation>({});
    } else {
        editor_env->current_entity->setComponent<Animation>(std::make_shared<Animation>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // animation

void componentAnimationMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Animation");

    auto move = editor_env->current_entity->getComponent<Animation>();
    bool enabled = move ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        animation::toggleAnimation(editor_env);
    }

    ImGui::End();
}

} // menu
