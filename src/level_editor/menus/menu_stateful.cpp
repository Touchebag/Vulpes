#include "menu_stateful.h"

#include "components/stateful.h"

#include "utils/log.h"

namespace menu {

namespace stateful {

void toggleStateful(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Stateful>()) {
        editor_env->current_entity->setComponent<Stateful>({});
    } else {
        editor_env->current_entity->setComponent<Stateful>(std::make_shared<Stateful>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

} // stateful

void componentStatefulMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Stateful");

    auto move = editor_env->current_entity->getComponent<Stateful>();
    bool enabled = move ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        stateful::toggleStateful(editor_env);
    }

    ImGui::End();
}

} // menu
