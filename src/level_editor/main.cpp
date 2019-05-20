#include <SFML/Graphics.hpp>

#include "file.h"
#include "log.h"
#include "base_entity.h"
#include "action.h"

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
    std::vector<std::shared_ptr<BaseEntity>> world_objects;

    Action current_action = Action::NONE;

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
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                                    current_action = Action::MOVE;
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
                        }
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    current_action = Action::NONE;
                    break;
                default:
                    break;
            }
        }

        if (current_action == Action::CAMERA_MOVE) {
            view_pos_x -= static_cast<float>(mouse_speed.first);
            view_pos_y -= static_cast<float>(mouse_speed.second);
        }

        if (current_action == Action::CAMERA_ZOOM) {
            view_size += static_cast<float>(mouse_speed.second * 5);
        } else if (current_action == Action::MOVE) {
            if (current_entity) {
                sf::Vector2i current_pos = current_entity->getPosition();
                current_entity->setPosition(util::X(current_pos.x + static_cast<int>(world_mouse_speed.first)), util::Y(current_pos.y + static_cast<int>(world_mouse_speed.second)));
            }
        }

        window.clear();

        if (current_entity) {
            sf::Text text;
            text.setFont(font);
            text.setFillColor(sf::Color::Red);

            // auto hitbox = current_entity->getAbsHitbox();
            auto pos = current_entity->getPosition();

            sf::View viewport({VIEW_POS_X, VIEW_POS_Y}, {VIEW_SIZE, VIEW_SIZE});
            window.setView(viewport);

            text.setString(std::string("X:") + std::to_string(pos.x) + std::string(" Y: ") + std::to_string(pos.y));
            text.setPosition(50, 50);
            window.draw(text);
        }

        sf::View viewport({view_pos_x, view_pos_y}, {view_size, view_size});
        window.setView(viewport);

        for (auto it = world_objects.begin(); it != world_objects.end(); ++it) {
            (*it)->render(window);
        }

        window.display();
    }
}
