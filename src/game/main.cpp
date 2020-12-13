#include "game/game_loop.h"
#include "system/world.h"
#include "system/system.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "Vulpes");
    System::getCamera()->setView(0.0, 0.0, 1000.0, 1000.0);

    window.setKeyRepeatEnabled(false);

    World::IWorldModify::loadWorldFromFile("world.json");
    World::IWorldModify::setEntrance(0);

    game_main(window);
}
