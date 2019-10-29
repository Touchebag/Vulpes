#pragma once

#include <memory>

#include "history.h"
#include "mouse.h"
#include "text_input.h"

#include "operation.h"

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

    Command(std::shared_ptr<History> history,
            std::shared_ptr<Operation> current_operation,
            std::shared_ptr<Mouse> mouse);

    void setLayer(World::Layer layer);

    void update();

    void add();
    void add(std::shared_ptr<BaseEntity> entity);
    void remove(std::shared_ptr<BaseEntity> entity);
    void copy(std::shared_ptr<BaseEntity> entity);

    void startCommand(Commands action);
    void stopCommand();

    void closeTextInput();

    World::Layer current_layer_ = World::Layer::MAIN;

    std::shared_ptr<BaseEntity> current_entity_;
    std::shared_ptr<TextInput> text_input_;

  private:
    std::shared_ptr<History> history_;
    std::shared_ptr<Operation> current_operation_;
    std::shared_ptr<Mouse> mouse_;

    Commands current_command_;
};
