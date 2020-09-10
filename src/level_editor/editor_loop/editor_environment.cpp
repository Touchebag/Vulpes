#include "editor_environment.h"

EditorEnvironment::EditorEnvironment(sf::RenderWindow& window) :
    mouse(std::make_shared<Mouse>(window)),
    command(history, current_operation, mouse) {
}
