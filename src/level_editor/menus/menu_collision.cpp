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

std::pair<int, int> getSize(std::shared_ptr<Collision> coll) {
    auto hbox = coll->getCollideable()->getHitbox();
    return {hbox->width_, hbox->height_};
}

void setSize(std::shared_ptr<Collision> coll, int width, int height) {
    auto collideable = coll->getCollideable();
    collideable->setHitbox(width, height);
}

} // collision

void MenuCollision::drawSize(std::shared_ptr<EditorEnvironment> editor_env, std::shared_ptr<Collision> coll) {
    if (coll) {
        ImGui::Checkbox("Match render size", &match_render_size_);

        auto size = collision::getSize(coll);

        if (match_render_size_) {
            if (auto render = editor_env->current_entity->getComponent<Rendering>()) {
                size = render->getSize();
            }
        }

        ImGui::InputInt("Width", &size.first);
        ImGui::InputInt("Height", &size.second);

        collision::setSize(coll, size.first, size.second);
    } else {
        ImGui::TextDisabled("Match render size");
        ImGui::TextDisabled("Width");
        ImGui::TextDisabled("Height");
    }
}

void MenuCollision::drawMenu(std::shared_ptr<EditorEnvironment> editor_env) {
    ImGui::Begin("Collision", nullptr, 0
            | ImGuiWindowFlags_AlwaysAutoResize
            );

    auto coll = editor_env->current_entity->getComponent<Collision>();
    bool enabled = coll ? true : false;

    if (ImGui::Checkbox("Enabled", &enabled)) {
        collision::toggleCollsion(editor_env);
    }

    drawSize(editor_env, coll);

    ImGui::End();
}
