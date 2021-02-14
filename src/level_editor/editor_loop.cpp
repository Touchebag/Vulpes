#include "utils/log.h"
#include "history.h"
#include "system/world.h"
#include "system/system.h"
#include "menu.h"
#include "components/rendering/rendering_text.h"
#include "editor_render.h"
#include "editor_loop/editor_environment.h"
#include "editor_loop/editor_mouse.h"

#include "operation.h"

bool render_current_layer_only = false;

int level_editor_main(sf::RenderWindow& window) {
    auto editor_env = EditorEnvironment::create_environment(window);

    auto renderInst = std::dynamic_pointer_cast<EditorRender>(System::getRender());
    renderInst->setEditorEnvironment(editor_env);

    auto cameraInst = System::getCamera();

    if (!renderInst) {
        throw std::runtime_error("Could not cast render instance");
    }

    if (auto player = World::IWorldRead::getPlayer().lock()) {
        editor_env->view_pos_x = static_cast<float>(player->trans_->getX());
        editor_env->view_pos_y = static_cast<float>(player->trans_->getY());
    }

    while (window.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
        }

        sf::Event event;
        while (window.pollEvent(event)) {
        editor_env->event = event;
            if (editor_env->menu) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                    editor_env->menu.reset();
                } else if (event.type == sf::Event::TextEntered) {
                    try {
                        char key = static_cast<char>(event.text.unicode);
                        int option = std::stoi(std::string(&key));
                        if (auto cmd = editor_env->menu->selectOption(option)) {
                            if (editor_env->current_entity) {
                                // Clear event buffer to avoid duplicate characters
                                sf::Event tmp_event;
                                while (window.pollEvent(tmp_event)) {};

                                editor_env->command->handleCommand(cmd.value());
                            }

                            // Close menu if we take an action
                            editor_env->menu.reset();
                        }
                    } catch (std::invalid_argument& e) {
                        LOGV("Invalid menu option, ignoring");
                    }
                }
            } else if (editor_env->command->text_input_) {
                switch (event.type) {
                    case sf::Event::Closed:
                        window.close();
                        break;
                    case sf::Event::TextEntered:
                        editor_env->command->text_input_->enterText(sf::String(event.text.unicode).toAnsiString());
                        break;
                    case sf::Event::KeyPressed:
                        switch (event.key.code) {
                            case sf::Keyboard::Enter:
                                editor_env->command->closeTextInput();
                                break;
                            default:
                                break;
                        }
                    default:
                        break;
                }
            } else {
                switch (event.type) {
                    case sf::Event::Closed:
                        window.close();
                        break;
                    case sf::Event::Resized:
                        System::getCamera()->setWindowSize(event.size.width, event.size.height);
                        break;
                    case sf::Event::MouseWheelScrolled:
                        EditorMouse::handleMouseScroll(editor_env);
                        break;
                    case sf::Event::KeyPressed:
                        switch (event.key.code) {
                            case sf::Keyboard::Key::P:
                                // Remove editor hud when switching back
                                for (auto it : editor_env->editor_entities) {
                                    World::IWorldModify::removeEntity(it);
                                }
                                return 0;
                                break;
                            case sf::Keyboard::Key::S:
                                World::getInstance<World::IWorldModify>().saveWorldToFile(World::IWorldRead::getCurrentRoomName());
                                break;
                            case sf::Keyboard::Key::A:
                                editor_env->command->add();
                                break;
                            case sf::Keyboard::Key::D:
                                editor_env->command->remove(editor_env->current_entity);
                                break;
                            case sf::Keyboard::Key::C:
                                editor_env->command->copy(editor_env->current_entity);
                                break;
                            case sf::Keyboard::Key::V:
                                render_current_layer_only = !render_current_layer_only;
                                renderInst->setParallaxEnabled(!renderInst->getParallaxEnabled());
                                break;
                            case sf::Keyboard::Key::Z:
                                if (editor_env->current_command == Command::Commands::NONE) {
                                    editor_env->history->undo();
                                }
                                break;
                            case sf::Keyboard::Key::J:
                                System::getCamera()->addTrauma(0.2f);
                                break;
                            case sf::Keyboard::Key::K:
                                System::getCamera()->addTrauma(0.5f);
                                break;
                            case sf::Keyboard::Key::L:
                                System::getCamera()->addTrauma(1.0f);
                                break;
                            case sf::Keyboard::Key::R:
                                if (editor_env->current_command == Command::Commands::NONE) {
                                    editor_env->history->redo();
                                }
                                break;
                            case sf::Keyboard::Key::E:
                                if (!editor_env->menu) {
                                    editor_env->menu = std::make_shared<Menu>(editor_env);
                                }
                                break;
                            case sf::Keyboard::Key::Num1:
                                renderInst->toggleHitboxRendering();
                                break;
                            case sf::Keyboard::Key::Num2:
                                renderInst->toggleEntranceRendering();
                                break;
                            // Move camera margins
                            case sf::Keyboard::Key::Left:
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                                    auto mouse_world_pos = editor_env->mouse->getMouseWorldPosition();
                                    auto camera_box = cameraInst->getCameraBox();
                                    camera_box.left_margin = mouse_world_pos.first;
                                    renderInst->setCameraBox(camera_box);
                                }
                                break;
                            case sf::Keyboard::Key::Right:
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                                    auto mouse_world_pos = editor_env->mouse->getMouseWorldPosition();
                                    auto camera_box = cameraInst->getCameraBox();
                                    camera_box.right_margin = mouse_world_pos.first;
                                    renderInst->setCameraBox(camera_box);
                                }
                                break;
                            case sf::Keyboard::Key::Up:
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                                    auto mouse_world_pos = editor_env->mouse->getMouseWorldPosition();
                                    auto camera_box = cameraInst->getCameraBox();
                                    camera_box.top_margin = mouse_world_pos.second;
                                    renderInst->setCameraBox(camera_box);
                                }
                                break;
                            case sf::Keyboard::Key::Down:
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                                    auto mouse_world_pos = editor_env->mouse->getMouseWorldPosition();
                                    auto camera_box = cameraInst->getCameraBox();
                                    camera_box.bottom_margin = mouse_world_pos.second;
                                    renderInst->setCameraBox(camera_box);
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    case sf::Event::MouseButtonPressed:
                        EditorMouse::handleMousePress(editor_env);
                        break;
                    case sf::Event::MouseButtonReleased:
                        EditorMouse::handleMouseRelease(editor_env);
                        break;
                    default:
                        break;
                }
            }
        }

        editor_env->command->update();

        if (editor_env->current_command == Command::Commands::CAMERA_MOVE) {
            auto mouse_dist = editor_env->mouse->getMouseDistance();
            editor_env->view_pos_x -= static_cast<float>(mouse_dist.first);
            editor_env->view_pos_y -= static_cast<float>(mouse_dist.second);
            editor_env->mouse->saveMousePosition();
        } else if (editor_env->current_command == Command::Commands::CAMERA_ZOOM) {
            auto mouse_dist = editor_env->mouse->getMouseDistance();
            editor_env->view_size += static_cast<float>(mouse_dist.second * 5);
            editor_env->mouse->saveMousePosition();
        }

        window.clear();

        cameraInst->setView(static_cast<float>(editor_env->view_pos_x), static_cast<float>(editor_env->view_pos_y), editor_env->view_size, editor_env->view_size);

        if (render_current_layer_only) {
            auto window_size = System::getCamera()->getWindowSize();
            sf::RenderTexture texture;
            texture.create(window_size.first, window_size.second);

            renderInst->renderLayer(texture, editor_env->current_layer);
            texture.display();

            window.draw(sf::Sprite(texture.getTexture()));
        } else {
            renderInst->render(window);
        }

        // Get this before changing to HUD view for proper coordinate mapping
        auto mouse_world_pos = editor_env->mouse->getMouseWorldPosition();

        auto window_size = System::getCamera()->getWindowSize();
        sf::RenderTexture texture;
        texture.create(window_size.first, window_size.second);

        sf::View viewport({editor_env->VIEW_POS_X, editor_env->VIEW_POS_Y}, {editor_env->VIEW_SIZE, editor_env->VIEW_SIZE});
        texture.setView(viewport);

        // Print current layer
        std::static_pointer_cast<RenderableText>(editor_env->editor_entities[EditorEnvironment::EditorEntities::LAYER_HUD_TEXT]->renderableEntity_)->setText(std::to_string(editor_env->current_layer));

        {
            std::static_pointer_cast<RenderableText>(editor_env->editor_entities[EditorEnvironment::EditorEntities::MOUSE_HUD_TEXT]->renderableEntity_)->setText(
                    std::string("Mouse X: ") + std::to_string(static_cast<int>(mouse_world_pos.first)) +
                    "\nMouse Y: " + std::to_string(static_cast<int>(mouse_world_pos.second)));
        }

        auto current_entity_hud_text = editor_env->editor_entities[EditorEnvironment::EditorEntities::CURRENT_ENTITY_HUD_TEXT];

        if (editor_env->current_entity) {
            auto transform = editor_env->current_entity->trans_;
            auto coll = editor_env->current_entity->collision_;

            if (transform && coll) {
                std::static_pointer_cast<RenderableText>(current_entity_hud_text->renderableEntity_)
                    ->setText(std::string("X:") + std::to_string(transform->getX()) +
                              " Y: " + std::to_string(transform->getY()) +
                              "\nW:" + std::to_string(coll->getCollideable()->getHitbox()->width_) +
                              " H: " + std::to_string(coll->getCollideable()->getHitbox()->height_));
            }
        } else {
            std::static_pointer_cast<RenderableText>(current_entity_hud_text->renderableEntity_)
                ->setText("");
        }

        // Needed for cursor positions to map correctly when zoomed
        texture.display();
        window.draw(sf::Sprite(texture.getTexture()));

        window.display();
    }

    return 0;
}
