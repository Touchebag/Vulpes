#include "menu_rendering.h"

#include "components/rendering/rendering.h"

#include "utils/log.h"

namespace menu {

namespace rendering {

constexpr int BUFFER_SIZE = 1024;
char texture_name_buf[BUFFER_SIZE];

void toggleRendering(std::shared_ptr<EditorEnvironment> editor_env) {
    System::IWorldModify::removeEntity(editor_env->current_entity);

    if (editor_env->current_entity->getComponent<Rendering>()) {
        editor_env->current_entity->setComponent<Rendering>({});
    } else {
        editor_env->current_entity->setComponent<Rendering>(std::make_shared<Rendering>(editor_env->current_entity->components_));
    }

    System::IWorldModify::addEntity(editor_env->current_entity);
}

void textureName(std::shared_ptr<Rendering> render, nlohmann::json j) {
    if (render) {
        std::string sprite_name = j["texture"];

        ImGui::Text("Texture");
        ImGui::InputTextWithHint(".png", sprite_name.c_str(), texture_name_buf, BUFFER_SIZE);
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            try {
                // Load texture and reset field
                render->loadTexture(texture_name_buf, File());
                texture_name_buf[0] = 0;
            } catch (std::invalid_argument& e) {
                LOGW("Unable to parse input text");
            }
        }
    } else {
        ImGui::TextDisabled("Texture");
    }
}

} // rendering

void componentRenderingMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Rendering");

    auto render = editor_env->current_entity->getComponent<Rendering>();
    bool enabled = render ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        rendering::toggleRendering(editor_env);
    }

    ImGui::Separator();

    nlohmann::json j;
    if (render) {
        j = render->outputToJson().value();
    }

    rendering::textureName(render, j);

    ImGui::End();
}

} // menu
