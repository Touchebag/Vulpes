#include "editor_environment.h"

#include "utils/log.h"

std::shared_ptr<EditorEnvironment> EditorEnvironment::create_environment(sf::RenderWindow& window) {
    auto env = std::shared_ptr<EditorEnvironment>(new EditorEnvironment(window));
    env->command = std::make_shared<Command>(env);

    env->view_pos_x = EditorEnvironment::VIEW_POS_X;
    env->view_pos_y = EditorEnvironment::VIEW_POS_Y;
    env->view_size = EditorEnvironment::VIEW_SIZE;

    return env;
}

EditorEnvironment::EditorEnvironment(sf::RenderWindow& window) :
    mouse(std::make_shared<Mouse>(window)) {
}

int EditorEnvironment::change_layer(bool towards_screen) {
    if (towards_screen && current_layer > -5) {
        return --current_layer;
    } else if (!towards_screen && current_layer < 5) {
        return ++current_layer;
    }

    return current_layer;
}
