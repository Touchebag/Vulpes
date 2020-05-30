#include "game/game_loop.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000,1000), "Vulpes");
    window.setKeyRepeatEnabled(false);

    game_main(window, "world.json");
}
