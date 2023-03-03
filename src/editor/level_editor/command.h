#pragma once

#include <memory>

#include "common/mouse.h"
#include "base_entity.h"

class EditorEnvironment;

class Command {
  public:
    enum class Commands {
        NONE,
        CAMERA_MOVE,
        CAMERA_ZOOM,
        MOVE,
        RESIZE,

        TOGGLE_RENDERABLE,
        TOGGLE_COLLISION,
        TOGGLE_MOVABLE,
        TOGGLE_PHYSICS,
        TOGGLE_ACTIONS,

        RENDERABLE_SPRITE_CHANGE,
        RENDERABLE_TILING_NONE,
        RENDERABLE_TILING_X,
        RENDERABLE_TILING_Y,
        RENDERABLE_TILING_XY,
    };

    explicit Command(std::weak_ptr<EditorEnvironment> editor_env);

    void update();

    void add();
    void add(std::shared_ptr<BaseEntity> entity);
    void remove();

    void copy();
    void paste();

    void handleCommand(Commands action);
    void stopCommand();

    int current_layer_ = 0;

  private:
    std::weak_ptr<EditorEnvironment> editor_env_;

    std::pair<int, int> original_entity_position_;
    std::pair<int, int> original_entity_size_;
};
