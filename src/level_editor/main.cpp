#include <SFML/Graphics.hpp>

#include "file.h"
#include "log.h"
#include "base_entity.h"
#include "history.h"
#include "action.h"
#include "world.h"
#include "render.h"

#include "commands/add.h"
#include "commands/delete.h"
#include "commands/move.h"
#include "commands/resize.h"

#define VIEW_POS_X 500.0
#define VIEW_POS_Y 500.0
#define VIEW_SIZE 1000.0

#define LEVEL_FILE_PATH "assets/world.json"

std::pair<int, int> mouse_pos = {0, 0};
std::pair<float, float> world_mouse_pos = {0.0, 0.0};

World::Layer current_layer = World::Layer::MAIN;
bool render_current_layer_only = false;

void resetMouseDistances(sf::RenderWindow& window) {
    sf::Vector2i tmp_pos = sf::Mouse::getPosition(window);
    mouse_pos = {tmp_pos.x, tmp_pos.y};

    sf::Vector2f tmp_world_pos = window.mapPixelToCoords(tmp_pos);
    world_mouse_pos = {tmp_world_pos.x, tmp_world_pos.y};
}

std::pair<std::pair<int, int>, std::pair<float, float>> getMouseDistances(sf::RenderWindow& window) {
        sf::Vector2i tmp_pos = sf::Mouse::getPosition(window);
        std::pair<int, int> mouse_distance = {tmp_pos.x - mouse_pos.first, tmp_pos.y - mouse_pos.second};

        sf::Vector2f tmp_world_pos = window.mapPixelToCoords(tmp_pos);
        std::pair<float, float> world_mouse_distance = {tmp_world_pos.x - world_mouse_pos.first, tmp_world_pos.y - world_mouse_pos.second};

        return {mouse_distance, world_mouse_distance};
}

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

    Action current_action = Action::NONE;

    History history;
    std::shared_ptr<command::Command> current_command;

    World::getInstance().loadWorldFromFile("assets/world.json");

    float view_pos_x = VIEW_POS_X;
    float view_pos_y = VIEW_POS_Y;
    float view_size = VIEW_SIZE;

    std::shared_ptr<BaseEntity> current_entity;

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
                                    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
                                    if (auto tmp = current_entity->renderableEntity_) {
                                        tmp->loadTexture("box.png");
                                    }
                                    entity->setHitbox(50, 50);
                                    entity->setPosition(static_cast<int>(world_mouse_pos.first), static_cast<int>(world_mouse_pos.second));
                                    World::getInstance().addEntity(entity, current_layer);

                                    current_command = std::make_shared<command::Add>(command::Add());
                                    current_command->entity_ = entity;
                                    current_command->layer_ = current_layer;

                                    history.addCommand(current_command);
                                }
                                break;
                            case sf::Keyboard::Key::D:
                                if (current_entity) {
                                    current_command = std::make_shared<command::Delete>(command::Delete());
                                    current_command->entity_ = current_entity;
                                    current_command->layer_ = current_layer;

                                    history.addCommand(current_command);

                                    World::getInstance().removeEntity(current_entity, current_layer);
                                    current_entity = nullptr;
                                }
                                break;
                            case sf::Keyboard::Key::C:
                                // TODO Copy constructor
                                if (current_entity) {
                                    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
                                    entity->loadFromJson(current_entity->outputToJson().value());
                                    entity->setPosition(static_cast<int>(world_mouse_pos.first), static_cast<int>(world_mouse_pos.second));
                                    World::getInstance().addEntity(entity, current_layer);

                                    current_command = std::make_shared<command::Add>(command::Add());
                                    current_command->entity_ = entity;
                                    current_command->layer_ = current_layer;

                                    history.addCommand(current_command);
                                }
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
                                if (current_action == Action::NONE) {
                                    history.undo();
                                }
                                break;
                            case sf::Keyboard::Key::R:
                                if (current_action == Action::NONE) {
                                    history.redo();
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    case sf::Event::MouseButtonPressed:
                        if (event.mouseButton.button == sf::Mouse::Button::Left) {
                            // Store current mouse position
                            resetMouseDistances(window);

                            current_entity = nullptr;

                            std::shared_ptr<Transform> tmp_trans = std::make_shared<Transform>();
                            tmp_trans->setPosition(static_cast<int>(world_mouse_pos.first), static_cast<int>(world_mouse_pos.second));
                            std::shared_ptr<Hitbox> tmp_hbox = std::make_shared<Hitbox>();
                            std::shared_ptr<Collision> tmp_coll = std::make_shared<Collision>(tmp_trans, tmp_hbox);
                            auto player = World::getInstance().getPlayer().lock();
                            if (player && player->collision_ && player->collision_->collides(tmp_coll)) {
                                current_entity = player;
                            } else {
                                for (auto it : World::getInstance().getWorldObjects(current_layer)) {
                                    if (it->collision_ && it->collision_->collides(tmp_coll)) {
                                        current_entity = it;
                                        // Only static objects' hitboxes should be adjustable
                                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                                            auto hbox = current_entity->getHitbox();

                                            current_command = std::make_shared<command::Resize>(command::Resize());
                                            current_command->entity_ = current_entity;
                                            current_command->before_ = {hbox.width_, hbox.height_};

                                            current_action = Action::RESIZE;
                                        }
                                        break;
                                    }
                                }
                            }

                            if (current_entity) {
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) &&
                                   !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                                    auto pos = current_entity->getPosition();

                                    current_command = std::make_shared<command::Move>(command::Move());
                                    current_command->entity_ = current_entity;
                                    current_command->before_ = {pos.x, pos.y};

                                    current_action = Action::MOVE;
                                }
                            }

                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
                                current_action = Action::CAMERA_MOVE;
                            }
                        } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
                            // Store current mouse position
                            resetMouseDistances(window);

                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
                                current_action = Action::CAMERA_ZOOM;
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
                        resetMouseDistances(window);
                        switch (current_action) {
                            case Action::MOVE:
                                {
                                    auto pos = current_entity->getPosition();

                                    current_command->after_ = {pos.x, pos.y};

                                    history.addCommand(current_command);
                                    break;
                                }
                            case Action::RESIZE:
                                {
                                    auto hbox = current_entity->getHitbox();

                                    current_command->after_ = {hbox.width_, hbox.height_};

                                    history.addCommand(current_command);
                                    break;
                                }
                            default:
                                break;
                        }
                        current_action = Action::NONE;
                        break;
                    default:
                        break;
                }
            }
        }

        if (current_action == Action::CAMERA_MOVE) {
            auto mouse_dist = getMouseDistances(window);
            view_pos_x -= static_cast<float>(mouse_dist.first.first);
            view_pos_y -= static_cast<float>(mouse_dist.first.second);
            resetMouseDistances(window);
        } else if (current_action == Action::CAMERA_ZOOM) {
            auto mouse_dist = getMouseDistances(window);
            view_size += static_cast<float>(mouse_dist.first.second * 5);
            resetMouseDistances(window);
        } else if (current_action == Action::MOVE) {
            auto mouse_dist = getMouseDistances(window);
            auto pos = current_command->before_;

            current_entity->setPosition(static_cast<int>(static_cast<float>(pos.first) + mouse_dist.second.first), static_cast<int>(static_cast<float>(pos.second) + mouse_dist.second.second));
        } else if (current_action == Action::RESIZE) {
            auto mouse_dist = getMouseDistances(window);
            auto hbox = current_command->before_;

            current_entity->setHitbox(static_cast<int>(static_cast<float>(hbox.first) + (mouse_dist.second.first * 2.0)), static_cast<int>(static_cast<float>(hbox.second) + (mouse_dist.second.second * 2.0)));
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

            text.setString(std::string("Mouse X: ") + std::to_string(world_mouse_pos.first));
            text.setPosition(500, 20);
            window.draw(text);
            text.setString(std::string("Mouse Y: ") + std::to_string(world_mouse_pos.second));
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
