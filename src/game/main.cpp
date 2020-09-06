#include "game/game_loop.h"
#include "system/world.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "Vulpes");
    window.setKeyRepeatEnabled(false);

    World::IWorldModify::loadWorldFromFile("world.json");
    World::IWorldModify::setEntrance(0);

    game_main(window);
}
