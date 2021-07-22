#include "menu_rendering.h"

#include "components/rendering/rendering.h"

#include "utils/log.h"

namespace menu {

namespace rendering {

void toggleRendering(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Rendering>()) {
        editor_env->current_entity->setComponent<Rendering>({});
    } else {
        editor_env->current_entity->setComponent<Rendering>(std::make_shared<Rendering>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // rendering

void componentRenderingMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Rendering");

    auto move = editor_env->current_entity->getComponent<Rendering>();
    bool enabled = move ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        rendering::toggleRendering(editor_env);
    }

    ImGui::End();
}

} // menu
