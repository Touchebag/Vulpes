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

#include "utils/log.h"

namespace menu {

namespace {

#define Q(x) #x

#define QUOTE(x) Q(x)

#define openComponentMenu(comp) { \
    if (editor_env->menus.comp->is_menu_open_) { \
        editor_env->menus.comp->drawMenu(editor_env); \
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
        editor_env->menus.component->is_menu_open_ = !editor_env->menus.component->is_menu_open_; \
    } \
}

#define recreateMenuInstances(component) { \
    editor_env->menus.component = std::make_shared<Menu##component>(); \
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

// This is used to see if the menu was opened this frame
bool open_last_frame;

} // namespace

void renderMenus(sf::RenderWindow& window, std::shared_ptr<EditorEnvironment> editor_env) {
    topMenu(window, editor_env);

    if (auto ent = editor_env->current_entity) {
        if (!open_last_frame) {
            executeFunctionOnAllComponents(recreateMenuInstances);
        }

        open_last_frame = true;

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
    } else {
        open_last_frame = false;
    }
}

} // menu
