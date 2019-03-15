#include <SFML/Graphics.hpp>

#include "file.h"
#include "log.h"
#include "base_entity.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "BLAAAAH");

    std::optional<nlohmann::json> j = file::loadJson("assets/world.json");
    std::vector<std::shared_ptr<BaseEntity>> world_objects;

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

    int mouse_x, mouse_y;

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
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Button::Right) {
                        mouse_x = event.mouseButton.x;
                        mouse_y = event.mouseButton.y;
                    }
                default:
                    break;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
            auto mouse_tmp_pos = sf::Mouse::getPosition(window);
            view_pos_x += static_cast<float>(mouse_x - mouse_tmp_pos.x);
            view_pos_y += static_cast<float>(mouse_y - mouse_tmp_pos.y);
            mouse_x = mouse_tmp_pos.x;
            mouse_y = mouse_tmp_pos.y;
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
