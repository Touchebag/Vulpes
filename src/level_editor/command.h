#pragma once

#include <memory>

#include "history.h"
#include "mouse.h"
#include "text_input.h"

#include "operation.h"

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

    Command(std::weak_ptr<EditorEnvironment> editor_env);

    void update();

    void add();
    void add(std::shared_ptr<BaseEntity> entity);
    void remove(std::shared_ptr<BaseEntity> entity);
    void copy(std::shared_ptr<BaseEntity> entity);

    void handleCommand(Commands action);
    void stopCommand();

    void closeTextInput();

    int current_layer_ = 0;

    std::shared_ptr<TextInput> text_input_;

  private:
    std::weak_ptr<EditorEnvironment> editor_env_;
};
