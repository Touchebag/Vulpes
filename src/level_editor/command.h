#pragma once

#include <memory>

#include "history.h"
#include "mouse.h"

#include "operation.h"

class Command {
  public:
    enum class Commands {
        NONE,
        CAMERA_MOVE,
        CAMERA_ZOOM,
        MOVE,
        RESIZE,
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

    World::Layer current_layer_ = World::Layer::MAIN;

    std::shared_ptr<BaseEntity> current_entity_;

  private:
    std::shared_ptr<History> history_;
    std::shared_ptr<Operation> current_operation_;
    std::shared_ptr<Mouse> mouse_;

    Commands current_command_;
};
