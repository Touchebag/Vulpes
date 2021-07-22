#include "menu_main.h"

#include "menu_collision.h"

namespace menu {

bool collision_menu_open = false;

void renderMenus(sf::RenderWindow& window, std::shared_ptr<EditorEnvironment> editor_env) {
    topMenu(window, editor_env);

    if (editor_env->current_entity) {
        auto transform = editor_env->current_entity->getComponent<Transform>();
        auto coll = editor_env->current_entity->getComponent<Collision>();

        ImGui::Begin("Entity", nullptr, 0
                | ImGuiWindowFlags_AlwaysAutoResize
                );

        if (transform && coll) {
            ImGui::Text("X: %i", transform->getX());
            ImGui::Text("Y: %i", transform->getY());
            ImGui::Text("Width: %i", coll->getCollideable()->getHitbox()->width_);
            ImGui::Text("Height: %i", coll->getCollideable()->getHitbox()->height_);

            ImGui::Spacing();
            ImGui::Text("Components");
            ImGui::Separator();

            ImGui::Bullet();
            if (ImGui::MenuItem("Collision")) {
                collision_menu_open = !collision_menu_open;
            }
        }

        if (collision_menu_open) {
            componentCollisionMenu(editor_env);
        }

        ImGui::End();
    }

    ImGui::SFML::Render(window);
}

} // menu
