#include "menu_ai.h"

#include "components/ai.h"

#include "utils/log.h"

namespace ai {

void toggleAi(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<AI>()) {
        editor_env->current_entity->setComponent<AI>({});
    } else {
        editor_env->current_entity->setComponent<AI>(std::make_shared<AI>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // ai

void MenuAI::drawMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Ai");

    bool enabled = editor_env->current_entity->getComponent<AI>() ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        ai::toggleAi(editor_env);
    }

    ImGui::End();
}
