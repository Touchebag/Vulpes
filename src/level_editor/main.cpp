#include <SFML/Graphics.hpp>

#include "file.h"
#include "log.h"
#include "base_entity.h"
#include "history.h"
#include "action.h"
#include "world.h"

#include "commands/add.h"
#include "commands/delete.h"
#include "commands/move.h"
#include "commands/resize.h"

#define VIEW_POS_X 500.0
#define VIEW_POS_Y 500.0
#define VIEW_SIZE 1000.0

#define LEVEL_FILE_PATH "assets/world.json"

std::pair<int, int> mouse_pos = {0, 0}, mouse_speed = {0, 0};
std::pair<float, float> world_mouse_pos = {0.0, 0.0}, world_mouse_speed = {0.0, 0.0};

void updateMousePositions(sf::RenderWindow& window) {
        std::pair<int, int> old_mouse_pos = mouse_pos;
        sf::Vector2i tmp_pos = sf::Mouse::getPosition(window);
        mouse_pos = {tmp_pos.x, tmp_pos.y};
        mouse_speed = {mouse_pos.first - old_mouse_pos.first, mouse_pos.second - old_mouse_pos.second};

        std::pair<float, float> old_world_mouse_pos = world_mouse_pos;
        sf::Vector2f tmp_world_pos = window.mapPixelToCoords(tmp_pos);
        world_mouse_pos = {tmp_world_pos.x, tmp_world_pos.y};
        world_mouse_speed = {world_mouse_pos.first - old_world_mouse_pos.first, world_mouse_pos.second - old_world_mouse_pos.second};
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "BLAAAAH");

    std::optional<nlohmann::json> j = file::loadJson(LEVEL_FILE_PATH);
    std::vector<std::shared_ptr<BaseEntity>>& world_objects = World::getInstance().getWorldObjects();

    Action current_action = Action::NONE;

    History history;
    std::shared_ptr<command::Command> current_command;

    if (j) {
        for (auto it : j.value()) {
            std::shared_ptr<BaseEntity> ent = std::make_shared<BaseEntity>();
            ent->loadFromJson(it);
            world_objects.push_back(std::move(ent));
        }
    }

    float view_pos_x = VIEW_POS_X;
    float view_pos_y = VIEW_POS_Y;
    float view_size = VIEW_SIZE;

    std::shared_ptr<BaseEntity> current_entity;

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        LOGE("Failed to load font");
        return EXIT_FAILURE;
    }

    while (window.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
        }

        updateMousePositions(window);

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseWheelScrolled:
                    if (event.mouseWheelScroll.delta > 0) {
                        view_size -= 200;
                    } else {
                        view_size += 200;
                    }
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::Key::S:
                            {
                                nlohmann::json j;
                                for (long unsigned int i = 0; i < world_objects.size(); i++) {
                                    auto object = world_objects.at(i)->outputToJson();
                                    if (object) {
                                        j.push_back(*object);
                                    } else {
                                        LOGE("Failed to parse object");
                                    }
                                }

                                if (file::storeJson(LEVEL_FILE_PATH, j)) {
                                    LOGD("World save successfully");
                                } else {
                                    LOGE("Failed to save json to file");
                                }

                            }
                            break;
                        case sf::Keyboard::Key::A:
                            {
                                std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
                                entity->loadTexture("box.png");
                                entity->setHitbox(util::X(50), util::Y(50));
                                entity->setPosition(util::X(static_cast<int>(world_mouse_pos.first)), util::Y(static_cast<int>(world_mouse_pos.second)));
                                world_objects.push_back(entity);

                                current_command = std::make_shared<command::Add>(command::Add());
                                current_command->entity_ = entity;

                                history.addCommand(current_command);
                            }
                            break;
                        case sf::Keyboard::Key::D:
                            if (current_entity) {
                                current_command = std::make_shared<command::Delete>(command::Delete());
                                current_command->entity_ = current_entity;

                                history.addCommand(current_command);

                                world_objects.erase(std::remove(world_objects.begin(), world_objects.end(), current_entity), world_objects.end());
                                current_entity = nullptr;
                            }
                            break;
                        case sf::Keyboard::Key::C:
                            if (current_entity) {
                                std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
                                auto hbox = current_entity->getHitbox();
                                entity->loadTexture("box.png");
                                entity->setHitbox(util::X(hbox.width_), util::Y(hbox.height_));
                                entity->setPosition(util::X(static_cast<int>(world_mouse_pos.first)), util::Y(static_cast<int>(world_mouse_pos.second)));
                                world_objects.push_back(entity);

                                current_command = std::make_shared<command::Add>(command::Add());
                                current_command->entity_ = entity;

                                history.addCommand(current_command);
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
                        Hitbox tmp_hbox;
                        tmp_hbox.setOffset({util::X(static_cast<int>(world_mouse_pos.first)), util::Y(static_cast<int>(world_mouse_pos.second))});

                        for (auto it : world_objects) {
                            if (it->getAbsHitbox().collides(tmp_hbox)) {
                                current_entity = it;
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                                    auto pos = current_entity->getPosition();

                                    current_command = std::make_shared<command::Move>(command::Move());
                                    current_command->entity_ = current_entity;
                                    current_command->before_ = {util::X(pos.x), util::Y(pos.y)};

                                    current_action = Action::MOVE;
                                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                                    auto hbox = current_entity->getHitbox();

                                    current_command = std::make_shared<command::Resize>(command::Resize());
                                    current_command->entity_ = current_entity;
                                    current_command->before_ = {util::X(hbox.width_), util::Y(hbox.height_)};

                                    current_action = Action::RESIZE;
                                }
                                break;
                            }
                            current_entity = nullptr;
                        }

                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
                            current_action = Action::CAMERA_MOVE;
                        }
                    } else if (event.mouseButton.button == sf::Mouse::Button::Right) {
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
                            current_action = Action::CAMERA_ZOOM;
                        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                            if (current_entity) {
                                auto hbox = current_entity->getHitbox();
                                int width = static_cast<int>(std::round(static_cast<float>(hbox.width_) / 5.0) * 5.0);
                                int height = static_cast<int>(std::round(static_cast<float>(hbox.height_) / 5.0) * 5.0);

                                current_entity->setHitbox(util::X(width), util::Y(height));
                            }
                        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                            if (current_entity) {
                                auto pos = current_entity->getPosition();
                                int x = static_cast<int>(std::round(static_cast<float>(pos.x) / 5.0) * 5.0);
                                int y = static_cast<int>(std::round(static_cast<float>(pos.y) / 5.0) * 5.0);

                                current_entity->setPosition(util::X(x), util::Y(y));
                            }
                        }
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    switch (current_action) {
                        case Action::MOVE:
                            {
                                auto pos = current_entity->getPosition();

                                current_command->after_ = {util::X(pos.x), util::Y(pos.y)};

                                history.addCommand(current_command);
                                break;
                            }
                        case Action::RESIZE:
                            {
                                auto hbox = current_entity->getHitbox();

                                current_command->after_ = {util::X(hbox.width_), util::Y(hbox.height_)};

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

        if (current_action == Action::CAMERA_MOVE) {
            view_pos_x -= static_cast<float>(mouse_speed.first);
            view_pos_y -= static_cast<float>(mouse_speed.second);
        } else if (current_action == Action::CAMERA_ZOOM) {
            view_size += static_cast<float>(mouse_speed.second * 5);
        } else if (current_action == Action::MOVE) {
            if (current_entity) {
                current_entity->setPosition(util::X(static_cast<int>(world_mouse_pos.first)), util::Y(static_cast<int>(world_mouse_pos.second)));
            }
        } else if (current_action == Action::RESIZE) {
            auto hbox = current_entity->getHitbox();

            current_entity->setHitbox(util::X(static_cast<int>(static_cast<float>(hbox.width_) + world_mouse_speed.first)), util::Y(static_cast<int>(static_cast<float>(hbox.height_) - world_mouse_speed.second)));
        }

        window.clear();

        for (auto it = world_objects.begin(); it != world_objects.end(); ++it) {
            (*it)->render(window);
        }

        if (current_entity) {
            sf::Text text;
            text.setFont(font);
            text.setFillColor(sf::Color::Red);

            // auto hitbox = current_entity->getAbsHitbox();
            auto pos = current_entity->getPosition();
            auto hbox = current_entity->getHitbox();

            sf::View viewport({VIEW_POS_X, VIEW_POS_Y}, {VIEW_SIZE, VIEW_SIZE});
            window.setView(viewport);

            text.setString(std::string("X:") + std::to_string(pos.x) + std::string(" Y: ") + std::to_string(pos.y));
            text.setPosition(50, 50);
            window.draw(text);

            text.setString(std::string("W:") + std::to_string(hbox.width_) + std::string(" H: ") + std::to_string(hbox.height_));
            text.setPosition(50, 100);
            window.draw(text);
        }

        window.display();

        sf::View viewport({view_pos_x, view_pos_y}, {view_size, view_size});
        window.setView(viewport);
    }
}
