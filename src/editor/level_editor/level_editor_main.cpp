#include "level_editor_main.h"

#include "system/world.h"
#include "system/system.h"
#include "components/rendering/rendering_text.h"
#include "common/editor_render.h"
#include "editor_environment.h"
#include "editor_mouse.h"
#include "menus/menu_main.h"
#include "common/common.h"

#include "utils/log.h"

bool render_current_layer_only = false;

int levelEditorMain(sf::RenderWindow& window) {
    sf::Clock delta_clock;

    auto editor_env = EditorEnvironment::create_environment(window);

    auto renderInst = std::dynamic_pointer_cast<EditorRender>(System::getRender());
    renderInst->setEditorEnvironment(editor_env);

    if (!renderInst) {
        throw std::runtime_error("Could not cast render instance");
    }

    auto cameraInst = System::getCamera();

    if (auto player = System::IWorldRead::getPlayer().lock()) {
        editor_env->view_pos_x = static_cast<float>(player->getComponent<Transform>()->getX());
        editor_env->view_pos_y = static_cast<float>(player->getComponent<Transform>()->getY());
    }

    // Trigger one update to initiate all entities
    System::IWorldModify::update();

    while (window.isOpen() && editor_common::getCurrentEditor() == editor_common::CurrentEditor::LEVEL) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            editor_env->current_entity.reset();
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            editor_env->event = event;
            ImGui::SFML::ProcessEvent(event);

            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::Resized:
                    System::getRender()->setWindowSize(window, event.size.width, event.size.height);
                    break;
                case sf::Event::MouseWheelScrolled:
                    EditorMouse::handleMouseScroll(editor_env);
                    break;
                case sf::Event::KeyPressed:
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                        switch (event.key.code) {
                            case sf::Keyboard::Key::P:
                                editor_common::setCurrentEditor(editor_common::CurrentEditor::GAME);
                                break;
                            case sf::Keyboard::Key::S:
                                System::IWorldModify::saveWorldToFile(System::IWorldRead::getCurrentRoomName());
                                break;
                            case sf::Keyboard::Key::A:
                                editor_env->command->add();
                                break;
                            case sf::Keyboard::Key::D:
                                editor_env->command->remove();
                                break;
                            case sf::Keyboard::Key::C:
                                editor_env->command->copy();
                                break;
                            case sf::Keyboard::Key::V:
                                editor_env->command->paste();
                                break;
                            case sf::Keyboard::Key::B:
                                render_current_layer_only = !render_current_layer_only;
                                renderInst->setParallaxEnabled(!renderInst->getParallaxEnabled());
                                break;
                            case sf::Keyboard::Key::Z:
                                if (editor_env->current_command == Command::Commands::NONE) {
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
                                }
                                break;
                            case sf::Keyboard::Key::E:
                                if (false) {
                                }
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

        editor_env->command->update();
        ImGui::SFML::Update(window, delta_clock.restart());

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

        menu::renderMenus(window, editor_env);

        ImGui::SFML::Render(window);

        window.display();
    }

    return 0;
}
