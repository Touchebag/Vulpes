#include "menu_main.h"

#include "menu_actions.h"
#include "menu_ai.h"
#include "menu_animation.h"
#include "menu_collision.h"
#include "menu_damageable.h"
#include "menu_death.h"
#include "menu_movement.h"
#include "menu_physics.h"
#include "menu_rendering.h"
#include "menu_stateful.h"
#include "menu_subentity.h"
#include "menu_transform.h"

#include "menu_component.h"

namespace menu {

namespace {

struct {
    MenuActions Actions;
    MenuAI AI;
    MenuAnimation Animation;
    MenuCollision Collision;
    MenuDamageable Damageable;
    MenuDeath Death;
    MenuMovement Movement;
    MenuPhysics Physics;
    MenuRendering Rendering;
    MenuStateful Stateful;
    MenuSubentity Subentity;
    MenuTransform Transform;
} open_menus;

#define Q(x) #x

#define QUOTE(x) Q(x)

#define openComponentMenu(comp) { \
    if (open_menus.comp.is_menu_open_) { \
        open_menus.comp.drawMenu(editor_env); \
    } \
}

#define componentEntry(component) { \
    if (ent->getComponent<component>()) { \
        ImGui::Bullet(); \
    } else { \
        ImGui::Spacing(); \
        ImGui::SameLine(); \
    } \
    \
    if (ImGui::MenuItem(QUOTE(component))) { \
        open_menus.component.is_menu_open_ = !open_menus.component.is_menu_open_; \
    } \
}

#define executeFunctionOnAllComponents(function) { \
        function(Actions); \
        function(AI); \
        function(Animation); \
        function(Collision); \
        function(Damageable); \
        function(Death); \
        function(Movement); \
        function(Physics); \
        function(Rendering); \
        function(Stateful); \
        function(Subentity); \
        function(Transform); \
}

} // namespace

void renderMenus(sf::RenderWindow& window, std::shared_ptr<EditorEnvironment> editor_env) {
    topMenu(window, editor_env);

    if (auto ent = editor_env->current_entity) {
        ImGui::Begin("Entity", nullptr, 0
                | ImGuiWindowFlags_AlwaysAutoResize
                );

        if (auto transform = editor_env->current_entity->getComponent<Transform>()) {
            ImGui::Text("X Pos: %i", transform->getX());
            ImGui::Text("Y Pos: %i", transform->getY());
        } else {
            ImGui::TextDisabled("X Pos: N/A");
            ImGui::TextDisabled("Y Pos: N/A");
        }

        if (auto coll = editor_env->current_entity->getComponent<Collision>()) {
            ImGui::Text("Width: %i", coll->getCollideable()->getHitbox()->width_);
            ImGui::Text("Height: %i", coll->getCollideable()->getHitbox()->height_);
        } else {
            ImGui::TextDisabled("Width: N/A");
            ImGui::TextDisabled("Height: N/A");
        }

        ImGui::Spacing();
        ImGui::Text("Components");
        ImGui::Separator();

        executeFunctionOnAllComponents(componentEntry);

        ImGui::End();

        executeFunctionOnAllComponents(openComponentMenu);
    }

    ImGui::SFML::Render(window);
}

} // menu
