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

namespace menu {

namespace {

struct OpenMenus {
    bool Actions = false;
    bool AI = false;
    bool Animation = false;
    bool Collision = false;
    bool Damageable = false;
    bool Death = false;
    bool Movement = false;
    bool Physics = false;
    bool Rendering = false;
    bool Stateful = false;
    bool Subentity = false;
    bool Transform = false;
} open_menus;

#define Q(x) #x

#define QUOTE(x) Q(x)

#define componentMenu(component) { \
    if (ent->getComponent<component>()) { \
        ImGui::Bullet(); \
    } else { \
        ImGui::Spacing(); \
        ImGui::SameLine(); \
    } \
    \
    if (ImGui::MenuItem(QUOTE(component))) { \
        open_menus.component = !open_menus.component; \
    } \
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

        componentMenu(Actions);
        componentMenu(AI);
        componentMenu(Animation);
        componentMenu(Collision);
        componentMenu(Damageable);
        componentMenu(Death);
        componentMenu(Movement);
        componentMenu(Physics);
        componentMenu(Rendering);
        componentMenu(Stateful);
        componentMenu(Subentity);
        componentMenu(Transform);

        ImGui::End();

        if (open_menus.Actions) {
            componentActionsMenu(editor_env);
        }
        if (open_menus.AI) {
            componentAiMenu(editor_env);
        }
        if (open_menus.Animation) {
            componentAnimationMenu(editor_env);
        }
        if (open_menus.Collision) {
            componentCollisionMenu(editor_env);
        }
        if (open_menus.Damageable) {
            componentDamageableMenu(editor_env);
        }
        if (open_menus.Death) {
            componentDeathMenu(editor_env);
        }
        if (open_menus.Movement) {
            componentMovementMenu(editor_env);
        }
        if (open_menus.Physics) {
            componentPhysicsMenu(editor_env);
        }
        if (open_menus.Rendering) {
            componentRenderingMenu(editor_env);
        }
        if (open_menus.Stateful) {
            componentStatefulMenu(editor_env);
        }
        if (open_menus.Subentity) {
            componentSubentityMenu(editor_env);
        }
        if (open_menus.Transform) {
            componentTransformMenu(editor_env);
        }
    }

    ImGui::SFML::Render(window);
}

} // menu
