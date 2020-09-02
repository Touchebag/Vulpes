#include "game/game_loop.h"
#include "system/world.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "Vulpes");
    window.setKeyRepeatEnabled(false);

    World::IWorldModify::loadWorldFromFile("world.json");
    // TODO Double loading to force spawn point
    // Remove when adding real entry point
    World::IWorldModify::loadRoom("world.json", 0);

    game_main(window);
}
