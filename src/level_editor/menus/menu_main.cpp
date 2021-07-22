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

bool actions_menu_open = false;
bool ai_menu_open = false;
bool animation_menu_open = false;
bool collision_menu_open = false;
bool damageable_menu_open = false;
bool death_menu_open = false;
bool movement_menu_open = false;
bool physics_menu_open = false;
bool rendering_menu_open = false;
bool stateful_menu_open = false;
bool subentity_menu_open = false;
bool transform_menu_open = false;

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
            if (ImGui::MenuItem("Actions")) {
                actions_menu_open = !actions_menu_open;
            }
            ImGui::Bullet();
            if (ImGui::MenuItem("Ai")) {
                ai_menu_open = !ai_menu_open;
            }
            ImGui::Bullet();
            if (ImGui::MenuItem("Animation")) {
                animation_menu_open = !animation_menu_open;
            }
            ImGui::Bullet();
            if (ImGui::MenuItem("Collision")) {
                collision_menu_open = !collision_menu_open;
            }
            ImGui::Bullet();
            if (ImGui::MenuItem("Damageable")) {
                damageable_menu_open = !damageable_menu_open;
            }
            ImGui::Bullet();
            if (ImGui::MenuItem("Death")) {
                death_menu_open = !death_menu_open;
            }
            ImGui::Bullet();
            if (ImGui::MenuItem("Movement")) {
                movement_menu_open = !movement_menu_open;
            }
            ImGui::Bullet();
            if (ImGui::MenuItem("Physics")) {
                physics_menu_open = !physics_menu_open;
            }
            ImGui::Bullet();
            if (ImGui::MenuItem("Rendering")) {
                rendering_menu_open = !rendering_menu_open;
            }
            ImGui::Bullet();
            if (ImGui::MenuItem("Stateful")) {
                stateful_menu_open = !stateful_menu_open;
            }
            ImGui::Bullet();
            if (ImGui::MenuItem("Subentity")) {
                subentity_menu_open = !subentity_menu_open;
            }
            ImGui::Bullet();
            if (ImGui::MenuItem("Transform")) {
                transform_menu_open = !transform_menu_open;
            }
        }

        if (actions_menu_open) {
            componentActionsMenu(editor_env);
        }
        if (ai_menu_open) {
            componentAiMenu(editor_env);
        }
        if (animation_menu_open) {
            componentAnimationMenu(editor_env);
        }
        if (collision_menu_open) {
            componentCollisionMenu(editor_env);
        }
        if (damageable_menu_open) {
            componentDamageableMenu(editor_env);
        }
        if (death_menu_open) {
            componentDeathMenu(editor_env);
        }
        if (movement_menu_open) {
            componentMovementMenu(editor_env);
        }
        if (physics_menu_open) {
            componentPhysicsMenu(editor_env);
        }
        if (rendering_menu_open) {
            componentRenderingMenu(editor_env);
        }
        if (stateful_menu_open) {
            componentStatefulMenu(editor_env);
        }
        if (subentity_menu_open) {
            componentSubentityMenu(editor_env);
        }
        if (transform_menu_open) {
            componentTransformMenu(editor_env);
        }

        ImGui::End();
    }

    ImGui::SFML::Render(window);
}

} // menu
