#include <SFML/Graphics.hpp>

#include "file.h"
#include "log.h"
#include "base_entity.h"
#include "action.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "BLAAAAH");

    std::optional<nlohmann::json> j = file::loadJson("assets/world.json");
    std::vector<std::shared_ptr<BaseEntity>> world_objects;

    Action current_action = Action::NONE;

    if (j) {
        for (auto it : j.value()) {
            std::shared_ptr<BaseEntity> ent = std::make_shared<BaseEntity>();
            ent->loadFromJson(it);
            world_objects.push_back(std::move(ent));
        }
    }

    float view_pos_x = 500.0;
    float view_pos_y = 500.0;
    float view_size = 1000.0;

    sf::Vector2i mouse_pos;

    std::shared_ptr<BaseEntity> current_entity;

    while (window.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
        }

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
                        case sf::Keyboard::Key::A:
                            if (current_action == Action::MOVE) {
                                current_action = Action::NONE;
                            } else {
                                current_action = Action::MOVE;
                            }
                            break;
                        default:
                            break;
                    }
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Button::Right) {
                        mouse_pos = sf::Mouse::getPosition(window);
                    } else if (event.mouseButton.button == sf::Mouse::Button::Left) {
                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

                        Hitbox tmp_hbox = {util::Right(worldPos.x), util::Left(worldPos.x), util::Top(worldPos.y), util::Bottom(worldPos.y)};
                        for (auto it : world_objects) {
                            if (it->getAbsHitbox().collides(tmp_hbox)) {
                                current_entity = it;
                                mouse_pos = sf::Mouse::getPosition(window);
                                break;
                            }
                            current_entity = nullptr;
                        }
                    }
                default:
                    break;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
            auto mouse_tmp_pos = sf::Mouse::getPosition(window);
            view_pos_x += static_cast<float>(mouse_pos.x - mouse_tmp_pos.x);
            view_pos_y += static_cast<float>(mouse_pos.y - mouse_tmp_pos.y);
            mouse_pos = mouse_tmp_pos;
        }

        if (current_action == Action::MOVE) {
            if (current_entity) {
                auto mouse_tmp_pos = sf::Mouse::getPosition(window);
                auto current_pos = current_entity->getPosition();
                current_entity->setPosition(util::X(current_pos.x - (mouse_pos.x - mouse_tmp_pos.x)), util::Y(current_pos.y - (mouse_pos.y - mouse_tmp_pos.y)));
                mouse_pos = mouse_tmp_pos;
            }
        }

        window.clear();

        sf::View viewport({view_pos_x, view_pos_y}, {view_size, view_size});
        window.setView(viewport);

        for (auto it = world_objects.begin(); it != world_objects.end(); ++it) {
            (*it)->render(window);
        }

        window.display();
    }
}
