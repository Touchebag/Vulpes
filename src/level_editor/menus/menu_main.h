#include <imgui.h>
#include <imgui-SFML.h>

#include "editor_loop/editor_environment.h"

#include "top_menu.h"

void render_menus(sf::RenderWindow& window, std::shared_ptr<EditorEnvironment> editor_env) {
    render_top_menu(window, editor_env);

    if (editor_env->current_entity) {
        auto transform = editor_env->current_entity->getComponent<Transform>();
        auto coll = editor_env->current_entity->getComponent<Collision>();

        if (transform && coll) {
            ImGui::Begin("Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
            ImGui::Text("X: %i", transform->getX());
            ImGui::Text("Y: %i", transform->getY());
            ImGui::Text("Width: %i", coll->getCollideable()->getHitbox()->width_);
            ImGui::Text("Height: %i", coll->getCollideable()->getHitbox()->height_);
            ImGui::End();
        }
    }

    ImGui::SFML::Render(window);
}
