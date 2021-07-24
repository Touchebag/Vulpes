#include "menu_physics.h"

#include "components/physics/physics.h"

#include "utils/log.h"

namespace physics {

void togglePhysics(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Physics>()) {
        editor_env->current_entity->setComponent<Physics>({});
    } else {
        editor_env->current_entity->setComponent<Physics>(std::make_shared<Physics>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // physics

void MenuPhysics::drawMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Physics");

    bool enabled = editor_env->current_entity->getComponent<Physics>() ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        physics::togglePhysics(editor_env);
    }

    ImGui::End();
}
