#include "game/game_loop.h"
#include "system/system.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "Vulpes");
    System::getRender()->setWindowSize(window, 1000, 1000);

    window.setKeyRepeatEnabled(false);

    System::IWorldModify::loadWorldFromFile("world.json");
    System::IWorldModify::setEntrance(0);

    game_main(window);
}
