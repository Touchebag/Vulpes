#include "editor_mouse.h"

#include "components/collision/movement/collision_static.h"

void EditorMouse::handleMousePress(std::shared_ptr<EditorEnvironment> editor_env) {
    if (editor_env->event.mouseButton.button == sf::Mouse::Button::Left) {
        // Store current mouse position
        editor_env->mouse->saveMousePosition();

        editor_env->current_entity = nullptr;

        auto mouse_world_pos = editor_env->mouse->getMouseWorldPosition();

        std::shared_ptr<Transform> tmp_trans = std::make_shared<Transform>();
        tmp_trans->setPosition(static_cast<int>(mouse_world_pos.first), static_cast<int>(mouse_world_pos.second));

        std::shared_ptr<CollisionStatic> tmp_coll = std::make_shared<CollisionStatic>(tmp_trans);
        tmp_coll->setHitbox(50, 50);

        auto player = World::getInstance<World::IWorldRead>().getPlayer().lock();
        if (player && player->collision_ && player->collision_->collides(tmp_coll)) {
            editor_env->current_entity = player;
        } else {
            for (auto it : World::getInstance<World::IWorldModify>().getWorldObjects()) {
                std::shared_ptr<Collision> other_coll;
                if (it->renderableEntity_ && (it->renderableEntity_->getLayer() == editor_env->current_layer)) {
                    other_coll = std::make_shared<CollisionStatic>(it->trans_);
                    auto size = it->renderableEntity_->getSize();
                    other_coll->setHitbox(size.first, size.second);
                } else if (it->collision_) {
                    other_coll = std::static_pointer_cast<Collision>(it->collision_);
                } else if (it->trans_) {
                    other_coll = std::make_shared<CollisionStatic>(it->trans_);
                    other_coll->setHitbox(50, 50);
                }

                if (other_coll && other_coll->collides(tmp_coll)) {
                    editor_env->current_entity = it;
                    break;
                }
            }
        }

        auto current_entity_hud_text = editor_env->editor_entities[EditorEnvironment::EditorEntities::CURRENT_ENTITY_HUD_TEXT];

        if (editor_env->current_entity) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                editor_env->command->handleCommand(Command::Commands::RESIZE);
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                editor_env->command->handleCommand(Command::Commands::MOVE);
            }

            auto transform = editor_env->current_entity->trans_;
            auto coll = editor_env->current_entity->collision_;

            if (transform && coll) {
                std::static_pointer_cast<RenderableText>(current_entity_hud_text->renderableEntity_)
                    ->setText(std::string("X:") + std::to_string(transform->getX()) +
                              " Y: " + std::to_string(transform->getY()) +
                              "\nW:" + std::to_string(coll->getHitbox()->width_) +
                              " H: " + std::to_string(coll->getHitbox()->height_));
            }
        } else {
            std::static_pointer_cast<RenderableText>(current_entity_hud_text->renderableEntity_)
                ->setText("");
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
            auto coll = editor_env->current_entity->collision_;
            if (editor_env->current_entity && coll) {
                int width = static_cast<int>(
                        std::round(static_cast<float>(coll->getHitbox()->width_) / 5.0) * 5.0);
                int height = static_cast<int>(
                        std::round(static_cast<float>(coll->getHitbox()->height_) / 5.0) * 5.0);

                editor_env->current_entity->collision_->setHitbox(width, height);
            }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
            if (editor_env->current_entity) {
                if(auto transform = editor_env->current_entity->trans_) {
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
