#include <SFML/Graphics.hpp>

#include "file.h"
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

    while (window.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            window.close();
        }

        window.clear();

        for (auto it = world_objects.begin(); it != world_objects.end(); ++it) {
            (*it)->render(window);
        }

        window.display();
    }
}
