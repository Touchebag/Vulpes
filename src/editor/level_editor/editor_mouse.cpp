#include "editor_mouse.h"

#include "components/collision/collideables/movement/collideable_static.h"
#include "system/system.h"

void EditorMouse::handleMousePress() {
    auto editor_env = EditorEnvironment::get_environment();

    if (editor_env->event.mouseButton.button == sf::Mouse::Button::Left) {
        // Store current mouse position
        editor_env->mouse->saveMousePosition();

        if (editor_env->current_entity) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                editor_env->command->handleCommand(Command::Commands::RESIZE);
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                editor_env->command->handleCommand(Command::Commands::MOVE);
            }
        } else {

            editor_env->current_entity = nullptr;

            auto mouse_world_pos = editor_env->mouse->getMouseWorldPosition();

            std::shared_ptr<ComponentStore> tmp_component_store = std::make_shared<ComponentStore>();
            tmp_component_store->setComponent<Transform>(std::make_shared<Transform>(tmp_component_store));
            tmp_component_store->getComponent<Transform>()->setPosition(static_cast<int>(mouse_world_pos.first), static_cast<int>(mouse_world_pos.second));

            std::shared_ptr<Collision> tmp_coll = std::make_shared<Collision>(tmp_component_store);
            tmp_coll->getCollideable()->setHitbox(50, 50);

            auto player = System::IWorldModify::getPlayer().lock();
            if (player && player->getComponent<Collision>() && player->getComponent<Collision>()->collides(tmp_coll)) {
                editor_env->current_entity = player;
            } else {
                for (auto it : System::IWorldModify::getWorldObjects()) {
                    std::shared_ptr<Collision> other_coll = std::make_shared<Collision>(it->components_);
                    if (it->getComponent<Rendering>() && (it->getComponent<Rendering>()->getLayer() == editor_env->current_layer)) {
                        auto size = it->getComponent<Rendering>()->getSize();
                        other_coll->getCollideable()->setHitbox(size.first, size.second);
                    } else if (it->getComponent<Collision>()) {
                        other_coll = it->getComponent<Collision>();
                    } else if (it->getComponent<Transform>()) {
                        other_coll->getCollideable()->setHitbox(50, 50);
                    }

                    if (other_coll && other_coll->collides(tmp_coll)) {
                        editor_env->current_entity = it;
                        break;
                    }
                }
            }
        }

    } else if (editor_env->event.mouseButton.button == sf::Mouse::Button::Right) {
        // Store current mouse position
        editor_env->mouse->saveMousePosition();

        editor_env->current_command = Command::Commands::CAMERA_MOVE;
    }
}

void EditorMouse::handleMouseRelease() {
    auto editor_env = EditorEnvironment::get_environment();

    editor_env->mouse->saveMousePosition();

    editor_env->command->stopCommand();
    editor_env->current_command = Command::Commands::NONE;
}

void EditorMouse::handleMouseScroll() {
    auto editor_env = EditorEnvironment::get_environment();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
        editor_env->current_layer = editor_env->change_layer(editor_env->event.mouseWheelScroll.delta > 0.0);
        editor_env->command->current_layer_ = editor_env->current_layer;
    } else {
        if (editor_env->event.mouseWheelScroll.delta > 0) {
            editor_env->view_size -= 200;
        } else {
            editor_env->view_size += 200;
        }
    }
}
