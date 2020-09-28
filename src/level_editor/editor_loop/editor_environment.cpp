#include "editor_environment.h"

std::shared_ptr<EditorEnvironment> EditorEnvironment::create_environment(sf::RenderWindow& window) {
    auto env = std::shared_ptr<EditorEnvironment>(new EditorEnvironment(window));
    env->command = std::make_shared<Command>(env);

    env->editor_entities[EditorEnvironment::EditorEntities::LAYER_HUD_TEXT] = editor_common::makeHudText({50, 20});
    env->editor_entities[EditorEnvironment::EditorEntities::MOUSE_HUD_TEXT] = editor_common::makeHudText({500, 20});
    env->editor_entities[EditorEnvironment::EditorEntities::CURRENT_ENTITY_HUD_TEXT] = editor_common::makeHudText({50, 100});

    env->view_pos_x = EditorEnvironment::VIEW_POS_X;
    env->view_pos_y = EditorEnvironment::VIEW_POS_Y;
    env->view_size = EditorEnvironment::VIEW_SIZE;

    return env;
}

EditorEnvironment::EditorEnvironment(sf::RenderWindow& window) :
    mouse(std::make_shared<Mouse>(window)) {
}

RenderableEntity::Layer EditorEnvironment::change_layer(bool towards_screen) {
    int layer_int = static_cast<int>(current_layer);
    if (towards_screen && layer_int > 0) {
        return static_cast<RenderableEntity::Layer>(layer_int - 1);
    } else if (!towards_screen && layer_int < static_cast<int>(RenderableEntity::Layer::MAX_LAYERS) - 1) {
        return static_cast<RenderableEntity::Layer>(layer_int + 1);
    }

    return current_layer;
}
