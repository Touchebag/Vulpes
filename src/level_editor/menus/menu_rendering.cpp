#include "menu_rendering.h"

#include "components/rendering/rendering.h"

#include "utils/log.h"

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

void tiling(std::shared_ptr<Rendering> render) {
    if (render) {
        int tiling_x = render->tiling_x_;
        int tiling_y = render->tiling_y_;

        ImGui::InputInt("Tiling X", &tiling_x);
        ImGui::InputInt("Tiling Y", &tiling_y);

        render->setTiling(tiling_x, tiling_y);
    } else {
        ImGui::TextDisabled("Tiling X");
        ImGui::TextDisabled("Tiling Y");
    }
}

std::pair<int, int> getCollisionSize(std::shared_ptr<EditorEnvironment> editor_env) {
    if (auto coll = editor_env->current_entity->getComponent<Collision>()) {
        auto hbox = coll->getCollideable()->getHitbox();
        return {hbox->width_, hbox->height_};
    }

    return {0, 0};
}

} // rendering

void MenuRendering::drawSize(std::shared_ptr<EditorEnvironment> editor_env, std::shared_ptr<Rendering> render) {
    if (render) {
        auto size = render->getSize();

        ImGui::Checkbox("Match collision size", &match_collision_size_);

        if (match_collision_size_) {
            size = rendering::getCollisionSize(editor_env);
        }

        ImGui::InputInt("Width", &size.first);
        ImGui::InputInt("Height", &size.second);

        render->setSize(size.first, size.second);
    } else {
        ImGui::TextDisabled("Width");
        ImGui::TextDisabled("Height");
    }
}

void MenuRendering::drawMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Rendering", nullptr, 0
            | ImGuiWindowFlags_AlwaysAutoResize
            );

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
    drawSize(editor_env, render);
    rendering::tiling(render);

    ImGui::End();
}
