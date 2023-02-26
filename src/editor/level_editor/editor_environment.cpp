#include "editor_environment.h"

#include "utils/log.h"

namespace editor_environment {

std::shared_ptr<EditorEnvironment> environment_ = {};

} // editor_environment

using editor_environment::environment_;

void EditorEnvironment::create_environment(sf::RenderWindow& window) {
    environment_ = std::shared_ptr<EditorEnvironment>(new EditorEnvironment(window));
    environment_->command = std::make_shared<Command>(environment_);

    environment_->view_pos_x = EditorEnvironment::VIEW_POS_X;
    environment_->view_pos_y = EditorEnvironment::VIEW_POS_Y;
    environment_->view_size = EditorEnvironment::VIEW_SIZE;
}

std::shared_ptr<EditorEnvironment> EditorEnvironment::get_environment() {
    return environment_;
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
