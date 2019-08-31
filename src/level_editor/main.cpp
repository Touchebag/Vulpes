#include <SFML/Graphics.hpp>

#include "file.h"
#include "log.h"
#include "base_entity.h"
#include "history.h"
#include "world.h"
#include "render.h"
#include "command.h"
#include "mouse.h"

#include "operations/add.h"
#include "operations/remove.h"
#include "operations/move.h"
#include "operations/resize.h"

#define VIEW_POS_X 500.0
#define VIEW_POS_Y 500.0
#define VIEW_SIZE 1000.0

#define LEVEL_FILE_PATH "assets/world.json"

World::Layer current_layer = World::Layer::MAIN;
bool render_current_layer_only = false;

World::Layer change_layer(bool towards_screen) {
    int layer_int = static_cast<int>(current_layer);
    if (towards_screen && layer_int > 0) {
        return static_cast<World::Layer>(layer_int - 1);
    } else if (!towards_screen && layer_int < static_cast<int>(World::Layer::MAX_LAYERS) - 1) {
        return static_cast<World::Layer>(layer_int + 1);
    }

    return current_layer;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "BLAAAAH");

    Render& renderInst = Render::getInstance();

    std::optional<nlohmann::json> j = file::loadJson(LEVEL_FILE_PATH);

    Command::Commands current_action = Command::Commands::NONE;

    std::shared_ptr<History> history = std::make_shared<History>();
    std::shared_ptr<Operation> current_operation;

    std::shared_ptr<Mouse> mouse = std::make_shared<Mouse>(window);

    World::getInstance().loadWorldFromFile("assets/world.json");

    float view_pos_x = VIEW_POS_X;
    float view_pos_y = VIEW_POS_Y;
    float view_size = VIEW_SIZE;

    std::shared_ptr<BaseEntity> current_entity;

    Command command{history, current_operation, mouse};

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        LOGE("Failed to load font");
        return EXIT_FAILURE;
    }

    bool entering_text = false;
    sf::String input_text;

    while (window.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (entering_text) {
                switch (event.type) {
                    case sf::Event::Closed:
                        window.close();
                        break;
                    case sf::Event::TextEntered:
                        input_text += event.text.unicode;
                        break;
                    case sf::Event::KeyPressed:
                        switch (event.key.code) {
                            case sf::Keyboard::Enter:
                                if (auto tmp = current_entity->renderableEntity_) {
                                    tmp->loadTexture(input_text.toAnsiString());
                                }
                                // TODO Remove when tiling issue fixed
                                current_entity->setHitbox(current_entity->getHitbox().width_, current_entity->getHitbox().height_);

                                input_text.clear();
                                entering_text = false;
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
                            command.current_layer_ = current_layer;
                        } else {
                            if (event.mouseWheelScroll.delta > 0) {
                                view_size -= 200;
                            } else {
                                view_size += 200;
                            }
                            break;
                        }
                    case sf::Event::KeyPressed:
                        switch (event.key.code) {
                            case sf::Keyboard::Key::S:
                                World::getInstance().saveWorldToFile(LEVEL_FILE_PATH);
                                break;
                            case sf::Keyboard::Key::A:
                                // Ctrl + mouse wheel scroll generates an extra key corresponding to A
                                // for some reason
                                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                                    command.add();
                                }
                                break;
                            case sf::Keyboard::Key::D:
                                command.remove(current_entity);
                                break;
                            case sf::Keyboard::Key::C:
                                command.copy(current_entity);
                                break;
                            case sf::Keyboard::Key::V:
                                render_current_layer_only = !render_current_layer_only;
                                renderInst.parallax_enabled_ = !renderInst.parallax_enabled_;
                                break;
                            case sf::Keyboard::Key::T:
                                if (current_entity) {
                                    entering_text = true;
                                    sf::Event tmp_event;
                                    // Clear event buffer to avoid duplicate characters
                                    while (window.pollEvent(tmp_event)) {};
                                }
                                break;
                            case sf::Keyboard::Key::Z:
                                if (current_action == Command::Commands::NONE) {
                                    history->undo();
                                }
                                break;
                            case sf::Keyboard::Key::R:
                                if (current_action == Command::Commands::NONE) {
                                    history->redo();
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    case sf::Event::MouseButtonPressed:
                        if (event.mouseButton.button == sf::Mouse::Button::Left) {
                            // Store current mouse position
                            mouse->saveMousePosition();

                            current_entity = nullptr;

                            auto mouse_world_pos = mouse->getMouseWorldPosition();

                            std::shared_ptr<Transform> tmp_trans = std::make_shared<Transform>();
                            tmp_trans->setPosition(static_cast<int>(mouse_world_pos.first), static_cast<int>(mouse_world_pos.second));

                            std::shared_ptr<Hitbox> tmp_hbox = std::make_shared<Hitbox>();
                            std::shared_ptr<Collision> tmp_coll = std::make_shared<Collision>(tmp_trans, tmp_hbox);

                            auto player = World::getInstance().getPlayer().lock();
                            if (player && player->collision_ && player->collision_->collides(tmp_coll)) {
                                current_entity = player;
                            } else {
                                for (auto it : World::getInstance().getWorldObjects(current_layer)) {
                                    std::shared_ptr<Collision> other_coll = nullptr;
                                    if (it->collision_) {
                                        other_coll = it->collision_;
                                    } else if (it->hitbox_ && it->trans_) {
                                        other_coll = std::make_shared<Collision>(it->trans_, it->hitbox_);
                                    }

                                    if (other_coll->collides(tmp_coll)) {
                                        current_entity = it;
                                        command.current_entity_ = current_entity;
                                        break;
                                    }
                                }
                            }

                            if (current_entity) {
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                                    command.startCommand(Command::Commands::RESIZE);
                                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                                    command.startCommand(Command::Commands::MOVE);
                                }
                            }

                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
                                current_action = Command::Commands::CAMERA_MOVE;
                            }
                        } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
                            // Store current mouse position
                            mouse->saveMousePosition();

                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
                                current_action = Command::Commands::CAMERA_ZOOM;
                            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                                if (current_entity) {
                                    auto hbox = current_entity->getHitbox();
                                    int width = static_cast<int>(std::round(static_cast<float>(hbox.width_) / 5.0) * 5.0);
                                    int height = static_cast<int>(std::round(static_cast<float>(hbox.height_) / 5.0) * 5.0);

                                    current_entity->setHitbox(width, height);
                                }
                            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                                if (current_entity) {
                                    auto pos = current_entity->getPosition();
                                    int x = static_cast<int>(std::round(static_cast<float>(pos.x) / 5.0) * 5.0);
                                    int y = static_cast<int>(std::round(static_cast<float>(pos.y) / 5.0) * 5.0);

                                    current_entity->setPosition(x, y);
                                }
                            }
                        }
                        break;
                    case sf::Event::MouseButtonReleased:
                        mouse->saveMousePosition();

                        command.stopCommand();
                        break;
                    default:
                        break;
                }
            }
        }

        command.update();

        if (current_action == Command::Commands::CAMERA_MOVE) {
            auto mouse_dist = mouse->getMouseDistance();
            view_pos_x -= static_cast<float>(mouse_dist.first);
            view_pos_y -= static_cast<float>(mouse_dist.second);
            mouse->saveMousePosition();
        } else if (current_action == Command::Commands::CAMERA_ZOOM) {
            auto mouse_dist = mouse->getMouseDistance();
            view_size += static_cast<float>(mouse_dist.second * 5);
            mouse->saveMousePosition();
        }

        window.clear();

        renderInst.setView(static_cast<float>(view_pos_x), static_cast<float>(view_pos_y), view_size, view_size);

        if (render_current_layer_only) {
            renderInst.renderLayer(window, current_layer);
        } else {
            renderInst.render(window);
        }

        sf::View old_viewport = renderInst.getView();
        sf::View viewport({VIEW_POS_X, VIEW_POS_Y}, {VIEW_SIZE, VIEW_SIZE});
        window.setView(viewport);

        // Print current layer
        {
            sf::Text text;
            text.setFont(font);
            text.setFillColor(sf::Color::Green);

            text.setString(World::getLayerString(current_layer));
            text.setPosition(50, 20);
            window.draw(text);

            auto mouse_world_pos = mouse->getMouseWorldPosition();
            text.setString(std::string("Mouse X: ") + std::to_string(mouse_world_pos.first));
            text.setPosition(500, 20);
            window.draw(text);
            text.setString(std::string("Mouse Y: ") + std::to_string(mouse_world_pos.second));
            text.setPosition(500, 50);
            window.draw(text);
        }

        if (current_entity) {
            sf::Text text;
            text.setFont(font);
            text.setFillColor(sf::Color::Red);

            // auto hitbox = current_entity->getAbsHitbox();
            auto pos = current_entity->getPosition();
            auto hbox = current_entity->getHitbox();

            text.setString(std::string("X:") + std::to_string(pos.x) + std::string(" Y: ") + std::to_string(pos.y));
            text.setPosition(50, 50);
            window.draw(text);

            text.setString(std::string("W:") + std::to_string(hbox.width_) + std::string(" H: ") + std::to_string(hbox.height_));
            text.setPosition(50, 100);
            window.draw(text);
        }

        if (entering_text) {
            sf::Text text;
            text.setFont(font);
            text.setFillColor(sf::Color::Red);

            text.setString("Sprite: ");
            text.setPosition(20, 300);
            window.draw(text);

            text.setString(input_text);
            text.setPosition(130, 300);
            window.draw(text);
        }

        // Needed for cursor positions to map correctly when zoomed
        // TODO Move text to be drawn by HUD layer in Render
        window.setView(old_viewport);
        window.display();
    }
}
