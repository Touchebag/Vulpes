#include "editor_mouse.h"

#include "components/collision/collideables/movement/collideable_static.h"

void EditorMouse::handleMousePress(std::shared_ptr<EditorEnvironment> editor_env) {
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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
            editor_env->current_command = Command::Commands::CAMERA_MOVE;
        }
    } else if (editor_env->event.mouseButton.button == sf::Mouse::Button::Right) {
        // Store current mouse position
        editor_env->mouse->saveMousePosition();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
            editor_env->current_command = Command::Commands::CAMERA_ZOOM;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
            auto coll = editor_env->current_entity->getComponent<Collision>();
            if (editor_env->current_entity && coll) {
                auto collideable = coll->getCollideable();
                int width = static_cast<int>(
                        std::round(static_cast<float>(collideable->getHitbox()->width_) / 5.0) * 5.0);
                int height = static_cast<int>(
                        std::round(static_cast<float>(collideable->getHitbox()->height_) / 5.0) * 5.0);

                editor_env->current_entity->getComponent<Collision>()->getCollideable()->setHitbox(width, height);
            }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
            if (editor_env->current_entity) {
                if(auto transform = editor_env->current_entity->getComponent<Transform>()) {
                    auto pos = transform->getPosition();
                    int x = static_cast<int>(std::round(static_cast<float>(pos.x) / 5.0) * 5.0);
                    int y = static_cast<int>(std::round(static_cast<float>(pos.y) / 5.0) * 5.0);

                    transform->setPosition(x, y);
                }
            }
        }
    }
}

void EditorMouse::handleMouseRelease(std::shared_ptr<EditorEnvironment> editor_env) {
    editor_env->mouse->saveMousePosition();

    editor_env->command->stopCommand();
    editor_env->current_command = Command::Commands::NONE;
}

void EditorMouse::handleMouseScroll(std::shared_ptr<EditorEnvironment> editor_env) {
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
