#include "utils/log.h"
#include "history.h"
#include "system/world.h"
#include "system/system.h"
#include "menu.h"
#include "components/rendering/rendering_text.h"
#include "components/collision/movement/collision_static.h"
#include "editor_render.h"
#include "editor_loop/editor_environment.h"

#include "operation.h"

#define VIEW_POS_X 500.0
#define VIEW_POS_Y 500.0
#define VIEW_SIZE 1000.0

RenderableEntity::Layer current_layer = RenderableEntity::Layer::MAIN;
bool render_current_layer_only = false;

RenderableEntity::Layer change_layer(bool towards_screen) {
    int layer_int = static_cast<int>(current_layer);
    if (towards_screen && layer_int > 0) {
        return static_cast<RenderableEntity::Layer>(layer_int - 1);
    } else if (!towards_screen && layer_int < static_cast<int>(RenderableEntity::Layer::MAX_LAYERS) - 1) {
        return static_cast<RenderableEntity::Layer>(layer_int + 1);
    }

    return current_layer;
}

int level_editor_main(sf::RenderWindow& window) {
    auto renderInst = std::dynamic_pointer_cast<EditorRender>(System::getRender());

    if (!renderInst) {
        throw std::runtime_error("Could not cast render instance");
    }

    float view_pos_x = VIEW_POS_X;
    float view_pos_y = VIEW_POS_Y;
    float view_size = VIEW_SIZE;

    if (auto player = World::IWorldRead::getPlayer().lock()) {
        view_pos_x = static_cast<float>(player->trans_->getX());
        view_pos_y = static_cast<float>(player->trans_->getY());
    }

    EditorEnvironment editor_env = {window};

    std::shared_ptr<Menu> menu;

    // TODO Create all of these directly in EditorEnv instead
    std::array<std::shared_ptr<BaseEntity>, EditorEnvironment::EditorEntities::MAX_ENTITIES> editor_entities;

    editor_entities[EditorEnvironment::EditorEntities::LAYER_HUD_TEXT] = editor_common::makeHudText({50, 20});
    editor_entities[EditorEnvironment::EditorEntities::MOUSE_HUD_TEXT] = editor_common::makeHudText({500, 20});
    editor_entities[EditorEnvironment::EditorEntities::CURRENT_ENTITY_HUD_TEXT] = editor_common::makeHudText({50, 50});

    while (window.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
        }

        sf::Event event;
        editor_env.event = event;
        while (window.pollEvent(event)) {
            if (menu) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                    menu.reset();
                } else if (event.type == sf::Event::TextEntered) {
                    try {
                        char key = static_cast<char>(event.text.unicode);
                        int option = std::stoi(std::string(&key));
                        if (auto cmd = menu->selectOption(option)) {
                            if (editor_env.current_entity) {
                                // Clear event buffer to avoid duplicate characters
                                sf::Event tmp_event;
                                while (window.pollEvent(tmp_event)) {};

                                editor_env.command.handleCommand(cmd.value());
                            }

                            // Close menu if we take an action
                            menu.reset();
                        }
                    } catch (std::invalid_argument& e) {
                        LOGV("Invalid menu option, ignoring");
                    }
                }
            } else if (editor_env.command.text_input_) {
                switch (event.type) {
                    case sf::Event::Closed:
                        window.close();
                        break;
                    case sf::Event::TextEntered:
                        editor_env.command.text_input_->enterText(sf::String(event.text.unicode).toAnsiString());
                        break;
                    case sf::Event::KeyPressed:
                        switch (event.key.code) {
                            case sf::Keyboard::Enter:
                                editor_env.command.closeTextInput();
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
                    case sf::Event::MouseWheelScrolled:
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                            current_layer = change_layer(event.mouseWheelScroll.delta > 0.0);
                            editor_env.command.current_layer_ = current_layer;
                        } else {
                            if (event.mouseWheelScroll.delta > 0) {
                                view_size -= 200;
                            } else {
                                view_size += 200;
                            }
                        }
                        break;
                    case sf::Event::KeyPressed:
                        switch (event.key.code) {
                            case sf::Keyboard::Key::P:
                                for (auto it : editor_entities) {
                                    World::IWorldModify::removeEntity(it);
                                }

                                World::getInstance<World::IWorldModify>().saveWorldToFile(World::IWorldRead::getCurrentRoomName());
                                return 0;
                                break;
                            case sf::Keyboard::Key::S:
                                World::getInstance<World::IWorldModify>().saveWorldToFile(World::IWorldRead::getCurrentRoomName());
                                break;
                            case sf::Keyboard::Key::A:
                                editor_env.command.add();
                                break;
                            case sf::Keyboard::Key::D:
                                editor_env.command.remove(editor_env.current_entity);
                                break;
                            case sf::Keyboard::Key::C:
                                editor_env.command.copy(editor_env.current_entity);
                                break;
                            case sf::Keyboard::Key::V:
                                render_current_layer_only = !render_current_layer_only;
                                renderInst->setParallaxEnabled(!renderInst->getParallaxEnabled());
                                break;
                            case sf::Keyboard::Key::Z:
                                if (editor_env.current_action == Command::Commands::NONE) {
                                    editor_env.history->undo();
                                }
                                break;
                            case sf::Keyboard::Key::R:
                                if (editor_env.current_action == Command::Commands::NONE) {
                                    editor_env.history->redo();
                                }
                                break;
                            case sf::Keyboard::Key::E:
                                if (!menu) {
                                    menu = std::make_shared<Menu>(editor_env.current_entity);
                                }
                                break;
                            case sf::Keyboard::Key::Num1:
                                renderInst->toggleHitboxRendering();
                                break;
                            default:
                                break;
                        }
                        break;
                    case sf::Event::MouseButtonPressed:
                        if (event.mouseButton.button == sf::Mouse::Button::Left) {
                            // Store current mouse position
                            editor_env.mouse->saveMousePosition();

                            editor_env.current_entity = nullptr;

                            auto mouse_world_pos = editor_env.mouse->getMouseWorldPosition();

                            std::shared_ptr<Transform> tmp_trans = std::make_shared<Transform>();
                            tmp_trans->setPosition(static_cast<int>(mouse_world_pos.first), static_cast<int>(mouse_world_pos.second));

                            std::shared_ptr<CollisionStatic> tmp_coll = std::make_shared<CollisionStatic>(tmp_trans);
                            tmp_coll->setHitbox(50, 50);

                            auto player = World::getInstance<World::IWorldRead>().getPlayer().lock();
                            if (player && player->collision_ && player->collision_->collides(tmp_coll)) {
                                editor_env.current_entity = player;
                                editor_env.command.current_entity_ = editor_env.current_entity;
                            } else {
                                for (auto it : World::getInstance<World::IWorldModify>().getWorldObjects()) {
                                    std::shared_ptr<Collision> other_coll;
                                    if (it->renderableEntity_ && (it->renderableEntity_->getLayer() == current_layer)) {
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
                                        editor_env.current_entity = it;
                                        editor_env.command.current_entity_ = editor_env.current_entity;
                                        break;
                                    }
                                }
                            }

                            auto current_entity_hud_text = editor_entities[EditorEnvironment::EditorEntities::CURRENT_ENTITY_HUD_TEXT];
                            World::getInstance<World::IWorldModify>().removeEntity(current_entity_hud_text);

                            if (editor_env.current_entity) {
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                                    editor_env.command.handleCommand(Command::Commands::RESIZE);
                                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                                    editor_env.command.handleCommand(Command::Commands::MOVE);
                                }
                                current_entity_hud_text = editor_common::makeHudText({50, 50});

                                auto transform = editor_env.current_entity->trans_;
                                auto coll = editor_env.current_entity->collision_;

                                if (transform && coll) {
                                    std::static_pointer_cast<RenderableText>(current_entity_hud_text->renderableEntity_)
                                        ->setText(std::string("X:") + std::to_string(transform->getX()) +
                                                  " Y: " + std::to_string(transform->getY()) +
                                                  "\nW:" + std::to_string(coll->getHitbox()->width_) +
                                                  " H: " + std::to_string(coll->getHitbox()->height_));
                                }
                            }

                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
                                editor_env.current_action = Command::Commands::CAMERA_MOVE;
                            }
                        } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
                            // Store current mouse position
                            editor_env.mouse->saveMousePosition();

                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
                                editor_env.current_action = Command::Commands::CAMERA_ZOOM;
                            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                                auto coll = editor_env.current_entity->collision_;
                                if (editor_env.current_entity && coll) {
                                    int width = static_cast<int>(
                                            std::round(static_cast<float>(coll->getHitbox()->width_) / 5.0) * 5.0);
                                    int height = static_cast<int>(
                                            std::round(static_cast<float>(coll->getHitbox()->height_) / 5.0) * 5.0);

                                    editor_env.current_entity->collision_->setHitbox(width, height);
                                }
                            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                                if (editor_env.current_entity) {
                                    if(auto transform = editor_env.current_entity->trans_) {
                                        auto pos = transform->getPosition();
                                        int x = static_cast<int>(std::round(static_cast<float>(pos.x) / 5.0) * 5.0);
                                        int y = static_cast<int>(std::round(static_cast<float>(pos.y) / 5.0) * 5.0);

                                        transform->setPosition(x, y);
                                    }
                                }
                            }
                        }
                        break;
                    case sf::Event::MouseButtonReleased:
                        editor_env.mouse->saveMousePosition();

                        editor_env.command.stopCommand();
                        editor_env.current_action = Command::Commands::NONE;
                        break;
                    default:
                        break;
                }
            }
        }

        editor_env.command.update();

        if (editor_env.current_action == Command::Commands::CAMERA_MOVE) {
            auto mouse_dist = editor_env.mouse->getMouseDistance();
            view_pos_x -= static_cast<float>(mouse_dist.first);
            view_pos_y -= static_cast<float>(mouse_dist.second);
            editor_env.mouse->saveMousePosition();
        } else if (editor_env.current_action == Command::Commands::CAMERA_ZOOM) {
            auto mouse_dist = editor_env.mouse->getMouseDistance();
            view_size += static_cast<float>(mouse_dist.second * 5);
            editor_env.mouse->saveMousePosition();
        }

        window.clear();

        renderInst->setView(static_cast<float>(view_pos_x), static_cast<float>(view_pos_y), view_size, view_size);

        if (render_current_layer_only) {
            renderInst->renderLayer(window, current_layer);
        } else {
            renderInst->render(window);
        }

        sf::View old_viewport = renderInst->getView();
        sf::View viewport({VIEW_POS_X, VIEW_POS_Y}, {VIEW_SIZE, VIEW_SIZE});
        window.setView(viewport);

        // Print current layer
        if (auto layer_string = RenderableEntity::getLayerString(current_layer)) {
            std::static_pointer_cast<RenderableText>(editor_entities[EditorEnvironment::EditorEntities::LAYER_HUD_TEXT]->renderableEntity_)->setText(layer_string.value());
        }

        {
            auto mouse_world_pos = editor_env.mouse->getMouseWorldPosition();
            std::static_pointer_cast<RenderableText>(editor_entities[EditorEnvironment::EditorEntities::MOUSE_HUD_TEXT]->renderableEntity_)->setText(
                    std::string("Mouse X: ") + std::to_string(mouse_world_pos.first) +
                    "\nMouse Y: " + std::to_string(mouse_world_pos.second));
        }

        // Needed for cursor positions to map correctly when zoomed
        window.setView(old_viewport);
        window.display();
    }

    return 0;
}
